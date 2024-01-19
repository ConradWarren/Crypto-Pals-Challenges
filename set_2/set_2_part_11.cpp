#include <iostream>
#include <string>
#include <random>
#include <cstring>
#include <unordered_set>

#include "../AES.h"

#define AES_128_BLOCK_SIZE 16

void Print_Byte_String(const std::string& byte_string){
	
	for(int i = 0; i<byte_string.size(); i++){
		
		if(std::isalnum(byte_string[i]) || std::isspace(byte_string[i]) || std::ispunct(byte_string[i])){
			std::cout<<byte_string[i];
		}else{
			unsigned char num = ((unsigned char)byte_string[i]) >> 4;
			std::cout<<"\\x"<< (char)((num > 9) ? num - 10 + 'a' : num + '0');
			num = ((int)(byte_string[i])) & 0xf;
			std::cout<< (char)((num > 9) ? num - 10 + 'a' : num + '0');
		}

	}
	std::cout<<'\n';
}

std::string XOR_Byte_String(const std::string& byte_str_1, const std::string& byte_str_2){
	
	if(byte_str_1.size() != byte_str_2.size()){
		std::cerr<<"Error : Invalid input sizes\n";
		return byte_str_1;
	}
	
	std::string result(byte_str_1.size(), 0);

	for(int i = 0; i<byte_str_1.size(); i++){
		result[i] = (unsigned char)byte_str_1[i] ^ (unsigned char)byte_str_2[i];
	}

	return result;
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

std::string Encrypt_AES_128_ECB(const std::string& plain_text, const std::string& key){
	
	std::string cipher_text;
	std::string plain_text_block;
	std::string round_keys(178, 0);
	std::string cipher_text_block(AES_128_BLOCK_SIZE, 0);
	
	aes_key_schedule_128((const unsigned char*)key.c_str(), (unsigned char*)round_keys.c_str());

	for(int i = 0; i<plain_text.size(); i += AES_128_BLOCK_SIZE){
		
		plain_text_block = (i + AES_128_BLOCK_SIZE > plain_text.size()) ? PKCS_7(plain_text.substr(i), AES_128_BLOCK_SIZE) : plain_text.substr(i, AES_128_BLOCK_SIZE);
		aes_encrypt_128((const unsigned char*)round_keys.c_str(), (const unsigned char*)plain_text_block.c_str(), (unsigned char*)cipher_text_block.c_str());
		cipher_text += cipher_text_block;
	}	
	
	return cipher_text;
}

std::string Decrypt_AES_128_ECB(const std::string& cipher_text, const std::string key){
	
	std::string plain_text;
	std::string plain_text_block(AES_128_BLOCK_SIZE, 0);
	std::string round_keys(178, 0);

	aes_key_schedule_128((const unsigned char*)key.c_str(), (unsigned char*)round_keys.c_str());

	for(int i = 0; i<cipher_text.size(); i += AES_128_BLOCK_SIZE){
		aes_decrypt_128((const unsigned char*)round_keys.c_str(), (const unsigned char*)cipher_text.substr(i, AES_BLOCK_SIZE).c_str(), (unsigned char*)plain_text_block.c_str());
		plain_text += plain_text_block;
	}

	return plain_text;
}

std::string Encrypt_AES_128_CBC(const std::string& plain_text, const std::string& key, const std::string& IV){
	
	std::string cipher_text;
	std::string plain_text_block;
	std::string round_keys(178, 0);
	std::string cipher_text_block(AES_128_BLOCK_SIZE, 0);
	std::string last_cipher_text_block = IV;

	aes_key_schedule_128((const unsigned char*)key.c_str(), (unsigned char*)round_keys.c_str());

	for(int i = 0; i<plain_text.size(); i += AES_128_BLOCK_SIZE){
		
		plain_text_block = (i + AES_128_BLOCK_SIZE > plain_text.size()) ? PKCS_7(plain_text.substr(i), AES_128_BLOCK_SIZE) : plain_text.substr(i, AES_128_BLOCK_SIZE); 
		plain_text_block = XOR_Byte_String(plain_text_block, last_cipher_text_block);
		aes_encrypt_128((const unsigned char*)round_keys.c_str(), (const unsigned char*)plain_text_block.c_str(), (unsigned char*)cipher_text_block.c_str());
		last_cipher_text_block = cipher_text_block;
		cipher_text += cipher_text_block;
	}
	
	return cipher_text;
}

std::string Decrypt_AES_128_CBC(const std::string& cipher_text, const std::string& key, const std::string& IV){

	std::string last_cipher_text_block = IV;
	std::string round_keys(178, 0);
	std::string plain_text_block(AES_128_BLOCK_SIZE, 0);
	std::string plain_text;
	
	aes_key_schedule_128((const unsigned char*)key.c_str(), (unsigned char*)round_keys.c_str());

	for(int i = 0; i<cipher_text.size(); i += AES_128_BLOCK_SIZE){
		aes_decrypt_128((const unsigned char*)round_keys.c_str(), (const unsigned char*)cipher_text.substr(i, AES_128_BLOCK_SIZE).c_str(), (unsigned char*)plain_text_block.c_str());
		plain_text += XOR_Byte_String(plain_text_block, last_cipher_text_block);
		last_cipher_text_block = cipher_text.substr(i, AES_128_BLOCK_SIZE);
	}
	
	return plain_text;
}

std::string Generate_Random_128_Key(){
	
	std::string key(AES_128_BLOCK_SIZE, 0);
	
	for(int i = 0; i<key.size(); i++){
		key[i] += (char)((long long)(std::rand() * std::rand() + std::rand())%256);
	}

	return key;
}

double Detect_AES_ECB(const std::string& cipher_text){
	
	if(cipher_text.size() % AES_128_BLOCK_SIZE){
		std::cerr<<"Error: Invalid Cipher text size\n";
	}

	std::unordered_set<std::string> unique_blocks;

	for(int i = 0; i<cipher_text.size(); i += AES_128_BLOCK_SIZE){
		unique_blocks.insert(cipher_text.substr(i, AES_128_BLOCK_SIZE));
	}

	return 1.0 - (double)unique_blocks.size() / (double)(cipher_text.size() / 16);
}

double Detect_AES_CBC(const std::string& cipher_text){
	
	return 1.0 - Detect_AES_ECB(cipher_text); 
}
