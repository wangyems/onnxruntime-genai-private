# python /code/Phi3_MoE/onnxruntime-genai-private/src/python/py/models/builder.py \
# -i /code/Phi3_MoE/hf_version_4k \
# -o /code/Phi3_MoE/phi3_moe_onnx_4k \
# -p fp16 \
# -e cuda --extra_options num_hidden_layers=2

python /code/Phi3_MoE/onnxruntime-genai-private/src/python/py/models/builder.py \
-i /code/Phi3_MoE/hf_version_128k \
-o /code/Phi3_MoE/phi3_moe_onnx_128k \
-p fp16 \
-e cuda --extra_options num_hidden_layers=2
