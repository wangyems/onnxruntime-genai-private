python /code/Phi3_MoE/onnxruntime-genai-private/src/python/py/models/builder.py \
-i /code/Phi3_MoE/hf_version \
-o /code/Phi3_MoE/phi3_moe_onnx \
-p fp32 \
-e cuda \
--extra_options num_hidden_layers=1
