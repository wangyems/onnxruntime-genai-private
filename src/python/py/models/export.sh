cd /wy/ORT_GENAI/wangye/mixtral/src/python/py/models
# python builder.py -m mistralai/Mixtral-8x7B-v0.1 -e cuda -p fp16 -o ./example-models/mixtral_rank_0 --extra_options world_size=2 rank=0 #num_hidden_layers=2
# python builder.py -m mistralai/Mixtral-8x7B-v0.1 -e cuda -p fp16 -o ./example-models/mixtral_rank_1 --extra_options world_size=2 rank=1 #num_hidden_layers=2

python builder.py -m mistralai/Mixtral-8x7B-v0.1 -e cuda -p fp16 -o ./example-models/mixtral_2layer_rank_0 --extra_options world_size=2 rank=0 num_hidden_layers=2
python builder.py -m mistralai/Mixtral-8x7B-v0.1 -e cuda -p fp16 -o ./example-models/mixtral_2layer_rank_1 --extra_options world_size=2 rank=1 num_hidden_layers=2
#python builder.py -m mistralai/Mixtral-8x7B-v0.1 -e cuda -p fp16 -o ./example-models/mixtral_2layer --extra_options num_hidden_layers=2