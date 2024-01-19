#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <openssl/evp.h>

#include "../AES.h"

std::string XOR_Byte_Strings(const std::string& str_1, const std::string& str_2){

	if(str_1.size() != str_2.size()){
		std::cerr<<"Error: XOR Function Requires strings of the same size"<<std::endl;
		exit(EXIT_FAILURE);
	}
	
	std::string result(str_1.size(), 0);
	
	for(int i = 0; i<str_1.size(); i++){
		result[i] = (unsigned char)str_1[i] ^ (unsigned char)str_2[i];
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

void Print_Byte_String(const std::string& byte_string){
	
	for(int i = 0; i<byte_string.size(); i++){
		
		if(std::isalnum(byte_string[i]) || std::isspace(byte_string[i]) || std::ispunct(byte_string[i])){
			std::cout<<byte_string[i];
		}
		else{
			int num = (((unsigned char)byte_string[i]) >> 4);	
			std::cout<<"\\x"<<(char)((num > 9) ? num - 10 + 'a' : '0' + num);
			num = ((int)byte_string[i]) & 0xf;	
			std::cout<< (char)((num > 9) ? num - 10 + 'a' : '0' + num);
		}
	}
	
	std::cout<<'\n';
}

std::string PKCS_7(const std::string& input, int block_size){
	
	if(input.size() >= block_size){
		return input;
	}
	
	char padding = (char)(block_size - input.size());
	std::string result(block_size, padding);
	
	memcpy(result.data(), input.data(), input.size());
	
	return result;
}

std::string Decrypt_AES_128_ECB_Block(const std::string& input, const std::string& key){
	
	std::string round_keys(176, '\0');
	std::string result(16, '\0');
	
	aes_key_schedule_128((const unsigned char*)key.c_str(), (unsigned char*)round_keys.c_str());
	aes_decrypt_128((const unsigned char*)round_keys.c_str(), (const unsigned char*)input.c_str(), (unsigned char*)result.c_str());
	return result;
}

std::string Decrypt_AES_128_CBC(const std::string& input_file_path, const std::string& key){
	
	std::ifstream file(input_file_path);
	std::string line;
	std::string input_text;
	std::string cipher_text;
	std::string last_cipher_text_block(16, '\0');
	std::string decrypted_text_block;
	std::string plain_text;

	if(!file.is_open()){
		std::cerr<<"Error: Could Not Open File\n";
		return "";
	}
	
	while(std::getline(file, line)){
		input_text += line;
	}

	cipher_text = Base_64_to_Bytes(input_text);
	for(int i = 0; i<cipher_text.size(); i += 16){	
		decrypted_text_block = Decrypt_AES_128_ECB_Block(cipher_text.substr(i, 16), key);
		plain_text += XOR_Byte_Strings(decrypted_text_block, last_cipher_text_block); 
		last_cipher_text_block = cipher_text.substr(i, 16);
	}
	
	return plain_text;
}

int main(int argc, char** argv){
	
	if(argc > 2){
		Print_Byte_String(Decrypt_AES_128_CBC(argv[1], argv[2]));
	}

	
	return 0;
}




