import os
import argparse
import win32com.client
import chardet 
import sys
  
def detect_encoding(file_path): 
    with open(file_path, 'rb') as file: 
        detector = chardet.universaldetector.UniversalDetector() 
        for line in file: 
            detector.feed(line) 
            if detector.done: 
                break
        detector.close() 
    return detector.result['encoding'] 

def get_version_number(file_path):
	information_parser = win32com.client.Dispatch("Scripting.FileSystemObject")
	return information_parser.GetFileVersion(file_path)


def get_list_exe(file_path):
	list_exe = []
	with open(file_path, 'r', encoding=detect_encoding(file_path)) as f:
		for line in f.readlines():
			line = os.path.normpath(line.replace("\n", ""))
			if line.endswith(".exe") and os.path.exists(line):
				list_exe.append(line)
			else:
				print("Skipping " + line)
	return list_exe


def clickable_path(file_path):
	return "file:///" + os.path.abspath(file_path).replace("\\", "/")


def export_version(list_exe, file_path):
	if len(list_exe) == 0:
		return
	print("\nWriting into output_file : ", file_path)
	with open(file_path, 'w') as f:
		for exe in list_exe:
			tool = os.path.basename(exe).replace(".exe", "")
			version = get_version_number(exe)
			line = tool + " - " + version + "\n"
			print("\t" + line.replace("\n", ""))
			f.write(line)



if __name__ == "__main__":
	default_input_file = 'Bench_Tool_exe_path.txt'
	default_output_file = 'fileVersionsToolsBench.txt'
	parser = argparse.ArgumentParser()
	parser.add_argument('-i', '--input', help='Path to txt file containing list of exe files')
	parser.add_argument('-o', '--output',  help='Path to generated file containing exe versions')
	args = parser.parse_args()

	input_file = (args.input + "\\" + default_input_file) if args.input and os.path.isdir(args.input) else default_input_file
	output_file = (args.output + "\\" + default_output_file) if args.output and os.path.isdir(args.output) else default_output_file
	print("input_file  = {}\noutput_file = {}".format(clickable_path(input_file), clickable_path(output_file)))

	all_exe = get_list_exe(input_file)
	export_version(all_exe, output_file)

