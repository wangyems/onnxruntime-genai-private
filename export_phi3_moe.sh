python /home/wangye/onnxruntime-genai-private/src/python/py/models/builder.py \
-i /home/sunghcho/onnx_models/phi-3-moe-rc3-0-8-20240610/hf_version \
-o /home/wangye/phi3_moe_onnx \
-p fp16 \
-e cuda \
--extra_options num_hidden_layers=1
