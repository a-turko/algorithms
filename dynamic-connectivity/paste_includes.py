import sys

def process_file(filename):
	
	with open(filename) as file_obj:
		for line in file_obj:
			tokens = line.split()
			
			if len(tokens) == 2 and tokens[0] == "#include" and len(tokens[1]) > 2 and tokens[1][0] == '"':
				new_file = tokens[1][1:-1]
				
				process_file(new_file)
				
			else:
				print(line.strip())
			



assert(len(sys.argv) == 2)

main_file = sys.argv[1]

process_file(main_file)
