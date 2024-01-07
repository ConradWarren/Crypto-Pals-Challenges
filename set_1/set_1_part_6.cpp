#include <iostream>
#include <fstream>
#include <string>
#include <vector>

int Hamming_Distance(const std::string& str_1, const std::string& str_2){
	
	int result = 0;
	
	for(int i = 0; i<str_1.size(); i++){
		result += __builtin_popcount((unsigned char)(str_1[i] ^ str_2[i]));	
	}
	
	return result;
}

int Base_64_Val(char x){
	
	if(x >= 'A' && x <= 'Z')return x - 'A';

	if(x >= 'a' && x <= 'z')return x - 'a' + 26;
	
	if(x >= '0' && x <= '9') return x - '0' + 52;
	
	if(x == '+') return 62;
	
	return 63;
}

std::string Base_64_to_Bytes(const std::string& base_64_str){
	
	std::string result((base_64_str.size()*6)%8 ? (base_64_str.size()*6)/8 + 1 : (base_64_str.size()*6)/8, 0);
	int idx = result.size()-1;
	int current_num = 0;
	int bits_read = 0;
	
	for(int i = base_64_str.size()-1; i>=0; i--){
		current_num |= ((Base_64_Val(base_64_str[i])) << bits_read);
		bits_read += 6;

		while(bits_read >= 8){
			result[idx] = current_num & 0xff;
			idx--;
			current_num >>= 8;
			bits_read -= 8;
		}
	}

	while(current_num){
		result[idx] = current_num & 0xff;
 		idx--;
		current_num >>= 8;
	}
	
	return result;
}

int Single_Bit_Score(const std::string& byte_string, int start_idx, int offset, char bit_key){
	
	int score = 0;
	char res = 0;

	for(int i = start_idx; i < byte_string.size(); i += offset){
		res = bit_key ^ byte_string[i];
		if((res >= 'a' && res <= 'z') || (res >= 'A' && res <= 'Z') || res == ' ' || res == '\n'){
			score++;
		}
	}
	
	return score;
}

std::string Decrypt_Multi_Byte_XOR(const std::string& byte_string,const std::string& key){
	
	std::string result;
	
	for(int i = 0; i<byte_string.size(); i++){
		result += byte_string[i] ^ key[i % key.size()];
	}

	return result;
}

std::string Decrypt_Base_64(const std::string& input_file_path){

	std::ifstream file(input_file_path);
	std::string line;
	std::string input_str;
	std::string byte_string;
	std::string key;
	std::vector<int> key_value_scores(29, 0);
	int key_size = 0;
	double best_hamming_distance = 100;

	while(std::getline(file, line)){
		input_str += line;
	}
	file.close();
	byte_string = Base_64_to_Bytes(input_str);
	
	for(int i = 1; i<= 40; i++){
		int hamming_distance = Hamming_Distance(byte_string.substr(0, i), byte_string.substr(i, i));
		hamming_distance += Hamming_Distance(byte_string.substr(0, i), byte_string.substr(i*2, i));
		hamming_distance += Hamming_Distance(byte_string.substr(0, i), byte_string.substr(i*3, i));
		hamming_distance += Hamming_Distance(byte_string.substr(i, i), byte_string.substr(i*2, i));
		hamming_distance += Hamming_Distance(byte_string.substr(i, i), byte_string.substr(i*3, i));
		hamming_distance += Hamming_Distance(byte_string.substr(i*2, i), byte_string.substr(i*3, i));	
		double score = (double)hamming_distance/(double)i;
		if(score < best_hamming_distance){
			best_hamming_distance = score;
			key_size = i;
		}
	}
	
	key_value_scores.resize(key_size, 0);

	for(int i = 0; i<key_size; i++){
		char key_byte = 0;
		for(int j = 0; j<256; j++){
			int score = Single_Bit_Score(byte_string,i,key_size, j);
			if(score > key_value_scores[i]){
				key_value_scores[i] = score;
				key_byte = (char)j;
			}
		}
		key += key_byte;
	}

	return Decrypt_Multi_Byte_XOR(byte_string, key);
}

int main(int argc, char** argv){
	
	if(argc > 1){
		std::cout<<Decrypt_Base_64(argv[1])<<'\n';
	}

	return 0;
}
