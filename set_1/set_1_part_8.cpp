#include <iostream>
#include <fstream>
#include <openssl/evp.h>
#include <unordered_set>

std::string Hex_to_Bytes(const std::string& input){
	
	std::string result((input.size()%2) ? input.size()/2 + 1 : input.size()/2, 0);
	int current_num = 0;
	int idx = result.size()-1;
	int bits_counted = 0;

	for(int i = input.size()-1; i >= 0; i--){
		
		current_num |= ((input[i] >= '0' && input[i] <= '9') ? input[i] - '0' : input[i] - 'a' + 10) << bits_counted;
		bits_counted += 4;

		if(bits_counted == 8){
			result[idx] = current_num;
			current_num = 0;
			bits_counted = 0;
			idx--;
		}
	}

	if(idx == 0){
		result[idx] = current_num;
	}

	return result;
}

double Count_Unique_Blocks(const std::string& cipher_text, int block_size){
	
	if(block_size <= 0){
		std::cerr<<"Invalid Block Size\n";
		return 2;
	}

	std::unordered_set<std::string> unique_blocks;
	
	for(int i = 0; i + block_size <= cipher_text.size(); i += block_size){
		unique_blocks.insert(cipher_text.substr(i, block_size));
	}
	
	if(cipher_text.size() % block_size){
		std::string padding(block_size - (cipher_text.size()%block_size), 0);
		unique_blocks.insert(cipher_text.substr(cipher_text.size() - 1 - (cipher_text.size()%block_size), cipher_text.size()%block_size) + padding);
	}

	return (double)unique_blocks.size() / (double)(cipher_text.size()/block_size + (cipher_text.size()%block_size ? 1 : 0));
}

std::string Detect_AES_128_ECB(const std::string& input_file_path){

	std::fstream file(input_file_path);
	std::string line;
	std::string cipher_text;
	std::string result;
	double score = 0;
	double best_score = 2;

	if(!file.is_open()){
		std::cerr<<"Error: Could not open file"<<std::endl;
		exit(EXIT_FAILURE);
	}

	while(std::getline(file, line)){
		cipher_text = Hex_to_Bytes(line);
		score = Count_Unique_Blocks(cipher_text, 16);
		if(score < best_score){
			best_score = score;
			result = line;
		}
	}

	file.close();
	
	return result;
}

int main(int argc, char** argv){
	
	if(argc > 1){
		std::cout<<Detect_AES_128_ECB(argv[1])<<'\n';
	}

	return 0;
}
