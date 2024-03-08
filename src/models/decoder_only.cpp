#include "../generators.h"
#include "decoder_only.h"

namespace Generators {

DecoderOnly_Model::DecoderOnly_Model(std::unique_ptr<Config> config, OrtEnv& ort_env, const ProviderOptions* provider_options)
    : Model{std::move(config), provider_options} {
  session_decoder_ = OrtSession::Create(ort_env, (config_->config_path / config_->model.decoder.filename).c_str(), session_options_.get());

  InitDeviceAllocator(*session_decoder_);
}

std::unique_ptr<State> DecoderOnly_Model::CreateState(RoamingArray<int32_t> sequence_lengths, const GeneratorParams& params) const {
  return std::make_unique<DecoderOnly_State>(*this, sequence_lengths, params);
}

DecoderOnly_State::DecoderOnly_State(const DecoderOnly_Model& model, RoamingArray<int32_t> sequence_lengths_unk, const GeneratorParams& params)
    : State{params},
      model_{model},
      position_metadata_{model, *this, sequence_lengths_unk} {
  input_ids_.Add();
  position_metadata_.AddAttentionMask();
  position_metadata_.AddPositionIDs();
  logits_.Add();
  kv_cache_.Add();
}

RoamingArray<float> DecoderOnly_State::Run(int current_length, RoamingArray<int32_t> next_tokens, RoamingArray<int32_t> next_indices) {
  if (first_run_) {
    first_run_ = false;
  } else {
    UpdateInputs(next_tokens, next_indices, current_length);
  }

  State::Run(*model_.session_decoder_, *model_.run_options_);
  return logits_.Get();
}

void DecoderOnly_State::UpdateInputs(const RoamingArray<int32_t>& next_tokens_unk, RoamingArray<int32_t> beam_indices, int current_length) {
  input_ids_.Update(next_tokens_unk);
  position_metadata_.UpdateAttentionMask(current_length);
  position_metadata_.UpdatePositionIDs(current_length);
  kv_cache_.Update(beam_indices.GetCPU(), current_length);
}

}  // namespace Generators
