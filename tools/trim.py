#!/usr/bin/

import sys, re

def check_usage(argv):
	if len(argv) < 2:
		sys.stderr.write(__file__ + ": Usage: <file name>\n")
		sys.exit(0)


def check_syntax(read_data):
	count = 0
	for ch in read_data:
		if ch == '{':
			count += 1
		elif ch == '}':
			count -= 1
	if count > 0:
		print("too many '{': ", count, ". syntax error.")
		return -1
	elif count < 0:
		print("too many '}': ", count, ". syntax error.")
		return -2
	else:
		print("syntax ok.")
		return 0


if __name__ == "__main__":

	check_usage( sys.argv )
	file_name = sys.argv[1]

	# ファイルから読み込み。そのとき、行頭の空白とタブ、行末の改行を削って格納。
	read_data = ""
	with open(file_name, "rt") as f:
		for line in f:
			line = line.lstrip(' \t')
			line = line.rstrip(' \t')
			line = line.replace("\t", "")
			line = line.replace("\n", "")
			line = re.sub("//.*", '', line)# コメント'//'行を削除。
			if len(line) == 0:
				continue
			elif line[0] == '#':# プリプロセッサ指令行は要らない。
				continue
			else:
				read_data += line
				# print(line)
	f.close()
	# print(read_data)
	# sys.exit(0)

	# '{' and '}' の対応を調べる。
	check_syntax(read_data)

	token = [""]
	token_index = 0
	i = 0
	while i < len(read_data):
		if read_data[i] is '{':
			count = 0
			while True:
				if i >= len(read_data):
					break

				if read_data[i] is '{':
					count += 1
				elif read_data[i] is '}':
					count -= 1
				i += 1

				if count <= 0:
					token.append("")
					token_index += 1
					break
		else:
			token[token_index] += read_data[i]
			i += 1
	
	with open(file_name + ".tfpfs.txt", "wt") as f:
		for t in token:
			if len(t):
				t = re.sub("/\*.+\*/", '', t)# コメント'/**/' を削除。
				f.write(t + ";\n")
	f.close()

	print("finish. output file name is '{:s}'".format( file_name + ".tfpfs.txt" ))
