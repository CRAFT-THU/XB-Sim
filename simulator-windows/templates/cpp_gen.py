import os, re, sys, traceback
from string import Template
import argparse

def str2bool(v):
    return v.lower() in ("yes", "true", "t", "1")


parser = argparse.ArgumentParser()
parser.add_argument('--cuda', default=False, type=str2bool, help='Use cuda to inference')
args = parser.parse_args()

conv_layers = 15
conv_buffer = 14
linear_layes = 4
linear_buffer = 4
use_cuda = args.cuda

conv_configs = [
	{'layer_num':1, 'image_size': 'IMAGE_SIZE_32', 'input_channel':'CHANNELS_3',
	 'output_channel':'CHANNELS_32', 'pooling_size':'POOLING_SIZE_1'},
	{'layer_num':2, 'image_size': 'IMAGE_SIZE_32', 'input_channel':'CHANNELS_32',
	 'output_channel':'CHANNELS_32', 'pooling_size':'POOLING_SIZE_1'},
	{'layer_num':3, 'image_size': 'IMAGE_SIZE_32', 'input_channel':'CHANNELS_32',
	 'output_channel':'CHANNELS_32', 'pooling_size':'POOLING_SIZE_1'},
	{'layer_num':4, 'image_size': 'IMAGE_SIZE_32', 'input_channel':'CHANNELS_32',
	 'output_channel':'CHANNELS_48', 'pooling_size':'POOLING_SIZE_1'},
	{'layer_num':5, 'image_size': 'IMAGE_SIZE_32', 'input_channel':'CHANNELS_48',
	 'output_channel':'CHANNELS_48', 'pooling_size':'POOLING_SIZE_2'},
	{'layer_num':6, 'image_size': 'IMAGE_SIZE_16', 'input_channel':'CHANNELS_48',
	 'output_channel':'CHANNELS_80', 'pooling_size':'POOLING_SIZE_1'},
	{'layer_num':7, 'image_size': 'IMAGE_SIZE_16', 'input_channel':'CHANNELS_80',
	 'output_channel':'CHANNELS_80', 'pooling_size':'POOLING_SIZE_1'},
	{'layer_num':8, 'image_size': 'IMAGE_SIZE_16', 'input_channel':'CHANNELS_80',
	 'output_channel':'CHANNELS_80', 'pooling_size':'POOLING_SIZE_1'},
	{'layer_num':9, 'image_size': 'IMAGE_SIZE_16', 'input_channel':'CHANNELS_80',
	 'output_channel':'CHANNELS_80', 'pooling_size':'POOLING_SIZE_1'},
	{'layer_num':10, 'image_size': 'IMAGE_SIZE_16', 'input_channel':'CHANNELS_80',
	 'output_channel':'CHANNELS_80', 'pooling_size':'POOLING_SIZE_2'},
	{'layer_num':11, 'image_size': 'IMAGE_SIZE_8', 'input_channel':'CHANNELS_80',
	 'output_channel':'CHANNELS_128', 'pooling_size':'POOLING_SIZE_1'},
	{'layer_num':12, 'image_size': 'IMAGE_SIZE_8', 'input_channel':'CHANNELS_128',
	 'output_channel':'CHANNELS_128', 'pooling_size':'POOLING_SIZE_1'},
	{'layer_num':13, 'image_size': 'IMAGE_SIZE_8', 'input_channel':'CHANNELS_128',
	 'output_channel':'CHANNELS_128', 'pooling_size':'POOLING_SIZE_1'},
	{'layer_num':14, 'image_size': 'IMAGE_SIZE_8', 'input_channel':'CHANNELS_128',
	 'output_channel':'CHANNELS_128', 'pooling_size':'POOLING_SIZE_1'},
	{'layer_num':15, 'image_size': 'IMAGE_SIZE_8', 'input_channel':'CHANNELS_128',
	 'output_channel':'CHANNELS_128', 'pooling_size':'POOLING_SIZE_8'}
]

conv_buffer_configs = [
	{'layer_num':1, 'image_size': 'IMAGE_SIZE_32', 'channels':'CHANNELS_32'},
	{'layer_num':2, 'image_size': 'IMAGE_SIZE_32', 'channels':'CHANNELS_32'},
	{'layer_num':3, 'image_size': 'IMAGE_SIZE_32', 'channels':'CHANNELS_32'},
	{'layer_num':4, 'image_size': 'IMAGE_SIZE_32', 'channels':'CHANNELS_48'},
	{'layer_num':5, 'image_size': 'IMAGE_SIZE_16', 'channels':'CHANNELS_48'},
	{'layer_num':6, 'image_size': 'IMAGE_SIZE_16', 'channels':'CHANNELS_80'},
	{'layer_num':7, 'image_size': 'IMAGE_SIZE_16', 'channels':'CHANNELS_80'},
	{'layer_num':8, 'image_size': 'IMAGE_SIZE_16', 'channels':'CHANNELS_80'},
	{'layer_num':9, 'image_size': 'IMAGE_SIZE_16', 'channels':'CHANNELS_80'},
	{'layer_num':10, 'image_size': 'IMAGE_SIZE_8', 'channels':'CHANNELS_80'},
	{'layer_num':11, 'image_size': 'IMAGE_SIZE_8', 'channels':'CHANNELS_128'},
	{'layer_num':12, 'image_size': 'IMAGE_SIZE_8', 'channels':'CHANNELS_128'},
	{'layer_num':13, 'image_size': 'IMAGE_SIZE_8', 'channels':'CHANNELS_128'},
	{'layer_num':14, 'image_size': 'IMAGE_SIZE_8', 'channels':'CHANNELS_128'}
]

# linear_configs = [
# 	{'layer_num':16, 'input_size':'INPUT_LINEAR_1', 'output_size':'INPUT_LINEAR_2'},
# 	{'layer_num':17, 'input_size':'INPUT_LINEAR_2', 'output_size':'OUTPUT_LINEAR'}
# ]
#
# linear_buffer_configs = [
# 	{'layer_num':15, 'input_size':'CHANNELS_128', 'output_size':'INPUT_LINEAR_1'},
# 	{'layer_num':16, 'input_size':'INPUT_LINEAR_2', 'output_size':'INPUT_LINEAR_2'}
# ]

linear_configs = [
	{'layer_num':16, 'input_size':'INPUT_LINEAR_1', 'output_size':'INPUT_LINEAR_2',
	 'crossbar_l':'CROSSBAR_L', 'crossbar_w':'CROSSBAR_W_512', 'length_offset':'(15*CROSSBAR_L)',
	 'width_offset':'(15*CROSSBAR_W)'},
	{'layer_num':17, 'input_size':'INPUT_LINEAR_2', 'output_size':'INPUT_LINEAR_3',
	 'crossbar_l':'CROSSBAR_L', 'crossbar_w':'CROSSBAR_W_4096',
	 'length_offset':'(16*CROSSBAR_L)', 'width_offset':'(15*CROSSBAR_W+CROSSBAR_W_512)'},
	{'layer_num':18, 'input_size':'INPUT_LINEAR_3', 'output_size':'INPUT_LINEAR_4',
	 'crossbar_l':'CROSSBAR_L_4608', 'crossbar_w':'CROSSBAR_W_2048',
	 'length_offset':'(17*CROSSBAR_L)', 'width_offset':'(15*CROSSBAR_W+CROSSBAR_W_512+CROSSBAR_W_4096)'},
	{'layer_num':19, 'input_size':'INPUT_LINEAR_4', 'output_size':'OUTPUT_LINEAR',
	 'crossbar_l':'CROSSBAR_L_2304', 'crossbar_w':'CROSSBAR_W',
	 'length_offset':'(17*CROSSBAR_L+CROSSBAR_L_4608)',
	 'width_offset':'(15*CROSSBAR_W+CROSSBAR_W_512+CROSSBAR_W_4096+CROSSBAR_W_2048)'}
]

linear_buffer_configs = [
	{'layer_num':15, 'input_size':'CHANNELS_128', 'output_size':'INPUT_LINEAR_1'},
	{'layer_num':16, 'input_size':'INPUT_LINEAR_2', 'output_size':'INPUT_LINEAR_2'},
	{'layer_num':17, 'input_size':'INPUT_LINEAR_3', 'output_size':'INPUT_LINEAR_3'},
	{'layer_num':18, 'input_size':'INPUT_LINEAR_4', 'output_size':'INPUT_LINEAR_4'}
]

prefix = '../generated/'

def mkdir(path):
	folder = os.path.exists(path)
	if not folder:
		os.makedirs(path)
		print('new generated folder')
	else:
		print('generated folder already exists')

mkdir(prefix)

for layer in range(0, conv_layers):
	output_name = r'stage_conv_%d.h' % (layer+1)
	output_file = open(prefix+output_name, 'w')

	lines = []

	template_file = open(r'conv_cpp.template', 'r')
	tmp1 = Template(template_file.read())

	if not use_cuda:
		lines.append(tmp1.substitute(
				crossbar_file = 'crossbar.h',
				layer_num = conv_configs[layer]['layer_num'],
				image_size = conv_configs[layer]['image_size'],
				input_channel = conv_configs[layer]['input_channel'],
				output_channel = conv_configs[layer]['output_channel'],
				pooling_size = conv_configs[layer]['pooling_size']))
	else:
		lines.append(tmp1.substitute(
				crossbar_file = 'crossbar_cuda.h',
				layer_num = conv_configs[layer]['layer_num'],
				image_size = conv_configs[layer]['image_size'],
				input_channel = conv_configs[layer]['input_channel'],
				output_channel = conv_configs[layer]['output_channel'],
				pooling_size = conv_configs[layer]['pooling_size']))
	
	output_file.writelines(lines)
	output_file.close()

	print('generate %s over.'%output_name)

for buff in range(0, conv_buffer):
	output_name = r'conv_buffer_%d.h' % (buff+1)
	output_file = open(prefix+output_name, 'w')

	lines = []

	template_file = open(r'conv_buffer_cpp.template', 'r')
	tmp1 = Template(template_file.read())

	lines.append(tmp1.substitute(
			layer_num = conv_buffer_configs[buff]['layer_num'],
			image_size = conv_buffer_configs[buff]['image_size'],
			channels = conv_buffer_configs[buff]['channels']))
	
	output_file.writelines(lines)
	output_file.close()

	print('generate %s over.'%output_name)

for linear in range(0, linear_layes):
	output_name = r'stage_linear_%d.h' % (linear_configs[linear]['layer_num'])
	output_file = open(prefix+output_name, 'w')

	lines = []

	template_file = open(r'linear_cpp.template', 'r')
	tmp1 = Template(template_file.read())

	if not use_cuda:
		lines.append(tmp1.substitute(
				crossbar_file = 'crossbar.h',
				layer_num = linear_configs[linear]['layer_num'],
				input_size = linear_configs[linear]['input_size'],
				output_size = linear_configs[linear]['output_size'],
				crossbar_l = linear_configs[linear]['crossbar_l'],
				crossbar_w = linear_configs[linear]['crossbar_w'],
				length_offset = linear_configs[linear]['length_offset'],
				width_offset = linear_configs[linear]['width_offset']))
	else:
		lines.append(tmp1.substitute(
				crossbar_file = 'crossbar_cuda.h',
				layer_num = linear_configs[linear]['layer_num'],
				input_size = linear_configs[linear]['input_size'],
				output_size = linear_configs[linear]['output_size'],
				crossbar_l = linear_configs[linear]['crossbar_l'],
				crossbar_w = linear_configs[linear]['crossbar_w'],
				length_offset = linear_configs[linear]['length_offset'],
				width_offset = linear_configs[linear]['width_offset']))
	
	output_file.writelines(lines)
	output_file.close()

	print('generate %s over.'%output_name)

for line_buff in range(0, linear_layes):
	output_name = r'linear_buffer_%d.h' % (linear_buffer_configs[line_buff]['layer_num'])
	output_file = open(prefix+output_name, 'w')

	lines = []

	template_file = open(r'linear_buffer_cpp.template', 'r')
	tmp1 = Template(template_file.read())

	lines.append(tmp1.substitute(
			layer_num = linear_buffer_configs[line_buff]['layer_num'],
			input_size = linear_buffer_configs[line_buff]['input_size'],
			output_size = linear_buffer_configs[line_buff]['output_size']))
	
	output_file.writelines(lines)
	output_file.close()

	print('generate %s over.'%output_name)
