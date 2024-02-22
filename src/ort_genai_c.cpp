// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.
#include "ort_genai_c.h"
#include <memory>
#include <onnxruntime_c_api.h>
#include <exception>
#include <cstdint>
#include <cstddef>
#include "generators.h"
#include "models/model.h"
#include "search.h"

namespace Generators {

std::unique_ptr<OrtEnv> g_ort_env;

OrtEnv& GetOrtEnv() {
  if (!g_ort_env) {
    Ort::InitApi();
    g_ort_env = OrtEnv::Create();
  }
  return *g_ort_env;
}

}  // namespace Generators

using Sequences = std::vector<std::vector<int32_t>>;

extern "C" {

#define OGA_TRY try {
#define OGA_CATCH                   \
  }                                 \
  catch (const std::exception& e) { \
    return new OgaResult{e.what()}; \
  }

struct OgaResult {
  explicit OgaResult(const char* what) : what_{what} {}
  std::string what_;
};

const char* OGA_API_CALL OgaResultGetError(OgaResult* result) {
  return result->what_.c_str();
}

struct OgaBuffer {
  std::unique_ptr<uint8_t[]> data_;
  std::vector<size_t> dims_;
  OgaDataType type_;
};

OgaDataType OGA_API_CALL OgaBufferGetType(const OgaBuffer* p) {
  return p->type_;
}

size_t OGA_API_CALL OgaBufferGetDimCount(const OgaBuffer* p) {
  return p->dims_.size();
}

OgaResult* OGA_API_CALL OgaBufferGetDims(const OgaBuffer* p, size_t* dims, size_t dim_count) {
  OGA_TRY
  if (dim_count != p->dims_.size())
    throw std::runtime_error("OgaBufferGetDims - passed in buffer size does not match dim count");

  std::copy(p->dims_.begin(), p->dims_.end(), dims);
  return nullptr;
  OGA_CATCH
}

const void* OGA_API_CALL OgaBufferGetData(const OgaBuffer* p) {
  return p->data_.get();
}

size_t OGA_API_CALL OgaSequencesCount(const OgaSequences* p) {
  return reinterpret_cast<const Sequences*>(p)->size();
}

size_t OGA_API_CALL OgaSequencesGetSequenceCount(const OgaSequences* p, size_t sequence) {
  return (*reinterpret_cast<const Sequences*>(p))[sequence].size();
}

const int32_t* OGA_API_CALL OgaSequencesGetSequenceData(const OgaSequences* p, size_t sequence) {
  return (*reinterpret_cast<const Sequences*>(p))[sequence].data();
}

OgaResult* OGA_API_CALL OgaCreateModel(const char* config_path, OgaDeviceType device_type, OgaModel** out) {
  OGA_TRY
  auto provider_options = Generators::GetDefaultProviderOptions(static_cast<Generators::DeviceType>(device_type));
  *out = reinterpret_cast<OgaModel*>(Generators::CreateModel(Generators::GetOrtEnv(), config_path, &provider_options).release());
  return nullptr;
  OGA_CATCH
}

OgaResult* OGA_API_CALL OgaCreateGeneratorParams(const OgaModel* model, OgaGeneratorParams** out) {
  OGA_TRY
  *out = reinterpret_cast<OgaGeneratorParams*>(new Generators::GeneratorParams(*reinterpret_cast<const Generators::Model*>(model)));
  return nullptr;
  OGA_CATCH
}

OgaResult* OGA_API_CALL OgaGeneratorParamsSetMaxLength(OgaGeneratorParams* params, size_t max_length) {
  reinterpret_cast<Generators::GeneratorParams*>(params)->max_length = static_cast<int>(max_length);
  return nullptr;
}

OgaResult* OGA_API_CALL OgaGeneratorParamsSetInputIDs(OgaGeneratorParams* oga_params, const int32_t* input_ids, size_t input_ids_count, size_t sequence_length, size_t batch_size) {
  OGA_TRY
  auto& params = *reinterpret_cast<Generators::GeneratorParams*>(oga_params);
  params.input_ids = std::span<const int32_t>(input_ids, input_ids_count);
  params.sequence_length = static_cast<int>(sequence_length);
  params.batch_size = static_cast<int>(batch_size);
  if (params.sequence_length * params.batch_size != input_ids_count)
    throw std::runtime_error("sequence length * batch size is not equal to input_ids_count");
  return nullptr;
  OGA_CATCH
}

OGA_EXPORT OgaResult* OGA_API_CALL OgaGeneratorParamsSetInputSequences(OgaGeneratorParams* oga_params, const OgaSequences* p_sequences) {
  OGA_TRY
  auto& params = *reinterpret_cast<Generators::GeneratorParams*>(oga_params);
  auto& sequences = *reinterpret_cast<const Sequences*>(p_sequences);
  const bool pad_right = true;

  size_t max_length = 0;
  for (auto& sequence : sequences)
    max_length = std::max(max_length, sequence.size());

  const size_t input_ids_count = max_length * sequences.size();

  params.input_ids_owner_ = std::make_unique<int32_t[]>(input_ids_count);
  auto input_ids = std::span<int32_t>(params.input_ids_owner_.get(), input_ids_count);
  params.input_ids = input_ids;
  params.sequence_length = static_cast<int>(max_length);
  params.batch_size = static_cast<int>(sequences.size());

  // Copy and pad the input sequences with pad_token_id
  for (size_t sequence_index = 0; sequence_index < sequences.size(); sequence_index++) {
    auto output_span = input_ids.subspan(sequence_index * max_length, max_length);
    auto input_span = sequences[sequence_index];

    auto pad_count = max_length - input_span.size();
    if (pad_right) {
      std::copy(input_span.begin(), input_span.end(), output_span.begin());
      std::fill(output_span.end() - pad_count, output_span.end(), params.pad_token_id);
    } else {
      std::fill(output_span.begin(), output_span.begin() + pad_count, params.pad_token_id);
      std::copy(input_span.begin(), input_span.end(), output_span.begin() + pad_count);
    }
  }

  return nullptr;
  OGA_CATCH
}

OgaResult* OGA_API_CALL OgaGenerate(const OgaModel* model, const OgaGeneratorParams* generator_params, OgaSequences** out) {
  OGA_TRY
  Sequences result = Generators::Generate(*reinterpret_cast<const Generators::Model*>(model), *reinterpret_cast<const Generators::GeneratorParams*>(generator_params));
  *out = reinterpret_cast<OgaSequences*>(std::make_unique<Sequences>(std::move(result)).release());
  return nullptr;
  OGA_CATCH
}

OgaResult* OgaCreateGenerator(const OgaModel* model, const OgaGeneratorParams* generator_params, OgaGenerator** out) {
  OGA_TRY
  *out = reinterpret_cast<OgaGenerator*>(CreateGenerator(*reinterpret_cast<const Generators::Model*>(model), *reinterpret_cast<const Generators::GeneratorParams*>(generator_params)).release());
  return nullptr;
  OGA_CATCH
}

bool OGA_API_CALL OgaGenerator_IsDone(const OgaGenerator* generator) {
  return reinterpret_cast<const Generators::Generator*>(generator)->IsDone();
}

OgaResult* OGA_API_CALL OgaGenerator_ComputeLogits(OgaGenerator* generator) {
  OGA_TRY
  reinterpret_cast<Generators::Generator*>(generator)->ComputeLogits();
  return nullptr;
  OGA_CATCH
}

OgaResult* OGA_API_CALL OgaGenerator_GenerateNextToken_Top(OgaGenerator* generator) {
  OGA_TRY
  reinterpret_cast<Generators::Generator*>(generator)->GenerateNextToken_Top();
  return nullptr;
  OGA_CATCH
}

size_t OGA_API_CALL OgaGenerator_GetSequenceLength(const OgaGenerator* oga_generator, size_t index) {
  auto& generator = *reinterpret_cast<const Generators::Generator*>(oga_generator);
  return generator.GetSequence(static_cast<int>(index)).GetCPU().size();
}

const int32_t* OGA_API_CALL OgaGenerator_GetSequence(const OgaGenerator* oga_generator, size_t index) {
  auto& generator = *reinterpret_cast<const Generators::Generator*>(oga_generator);
  return generator.GetSequence(static_cast<int>(index)).GetCPU().data();
}

OgaResult* OGA_API_CALL OgaCreateTokenizer(const OgaModel* model, OgaTokenizer** out) {
  OGA_TRY
  *out = reinterpret_cast<OgaTokenizer*>(reinterpret_cast<const Generators::Model*>(model)->CreateTokenizer().release());
  return nullptr;
  OGA_CATCH
}

OgaResult* OGA_API_CALL OgaTokenizerEncodeBatch(const OgaTokenizer* p, const char* const* strings, size_t count, OgaSequences** out) {
  OGA_TRY
  auto& tokenizer = *reinterpret_cast<const Generators::Tokenizer*>(p);
  auto sequences = std::make_unique<Sequences>();
  for (size_t i = 0; i < count; i++) {
    sequences->emplace_back(tokenizer.Encode(strings[i]));
  }
  *out = reinterpret_cast<OgaSequences*>(sequences.release());
  return nullptr;
  OGA_CATCH
}

OgaResult* OGA_API_CALL OgaTokenizerDecodeBatch(const OgaTokenizer* p, const OgaSequences* p_sequences, const char* const** out_strings) {
  OGA_TRY
  auto& tokenizer = *reinterpret_cast<const Generators::Tokenizer*>(p);
  auto& sequences = *reinterpret_cast<const Sequences*>(p_sequences);

  std::vector<std::unique_ptr<char[]>> strings;
  for (size_t i = 0; i < sequences.size(); i++) {
    auto string = tokenizer.Decode(sequences[i]);
    auto length = string.length() + 1;
    auto& cstr_buffer = strings.emplace_back(std::make_unique<char[]>(length));
#ifdef _MSC_VER
    strcpy_s(cstr_buffer.get(), length, string.c_str());
#else
    strncpy(cstr_buffer.get(), string.c_str(), length);
    cstr_buffer[length] = 0;
#endif
  }

  auto strings_buffer = std::make_unique<const char*[]>(strings.size());
  for (size_t i = 0; i < strings.size(); i++) {
    strings_buffer[i] = strings[i].release();
  }
  *out_strings = strings_buffer.release();
  return nullptr;
  OGA_CATCH
}

void OGA_API_CALL OgaTokenizerDestroyStrings(const char* const* strings, size_t count) {
  for (size_t i = 0; i < count; i++)
    delete strings[i];
  delete strings;
}

OgaResult* OGA_API_CALL OgaTokenizerDecode(const OgaTokenizer* p, const int32_t* tokens, size_t token_count, const char** out_string) {
  OGA_TRY
  auto& tokenizer = *reinterpret_cast<const Generators::Tokenizer*>(p);

  auto string = tokenizer.Decode({tokens, token_count});
  auto length = string.length() + 1;
  auto cstr_buffer = std::make_unique<char[]>(length);
#if _MSC_VER
  strcpy_s(cstr_buffer.get(), length, string.c_str());
#else
  strncpy(cstr_buffer.get(), string.c_str(), length);
  cstr_buffer[length] = 0;
#endif
  *out_string = cstr_buffer.release();
  return nullptr;
  OGA_CATCH
}

OgaResult* OGA_API_CALL OgaCreateTokenizerStream(const OgaTokenizer* p, OgaTokenizerStream** out) {
  OGA_TRY
  *out = reinterpret_cast<OgaTokenizerStream*>(reinterpret_cast<const Generators::Tokenizer*>(p)->CreateStream().release());
  return nullptr;
  OGA_CATCH
}

OgaResult* OGA_API_CALL OgaTokenizerStreamDecode(OgaTokenizerStream* p, int32_t token, const char** out) {
  OGA_TRY
  *out = reinterpret_cast<Generators::TokenizerStream*>(p)->Decode(token).c_str();
  return nullptr;
  OGA_CATCH
}

void OGA_API_CALL OgaDestroyResult(OgaResult* p) {
  delete p;
}

void OGA_API_CALL OgaDestroyString(const char* p) {
  delete p;
}

void OGA_API_CALL OgaDestroyBuffer(OgaBuffer* p) {
  delete p;
}

void OGA_API_CALL OgaDestroySequences(OgaSequences* p) {
  delete reinterpret_cast<Sequences*>(p);
}

void OGA_API_CALL OgaDestroyModel(OgaModel* p) {
  delete reinterpret_cast<Generators::Model*>(p);
}

void OGA_API_CALL OgaDestroyGeneratorParams(OgaGeneratorParams* p) {
  delete reinterpret_cast<Generators::GeneratorParams*>(p);
}

void OGA_API_CALL OgaDestroyGenerator(OgaGenerator* p) {
  delete reinterpret_cast<Generators::Generator*>(p);
}

void OGA_API_CALL OgaDestroyTokenizer(OgaTokenizer* p) {
  delete reinterpret_cast<Generators::Tokenizer*>(p);
}

void OGA_API_CALL OgaDestroyTokenizerStream(OgaTokenizerStream* p) {
  delete reinterpret_cast<Generators::TokenizerStream*>(p);
}
}
