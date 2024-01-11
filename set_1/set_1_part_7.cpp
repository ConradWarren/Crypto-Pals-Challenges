#include <iostream>
#include <fstream>
#include <openssl/evp.h>

inline int Base_64_Val(char x){

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

		if(bits_read >= 8){
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

std::string Decrypt_AES_128_ECB(const std::string& input_file_path, const std::string& key){
	
	std::ifstream file(input_file_path);
	std::string line;
	std::string input_str;
	std::string cypher;
	std::string decryptedText;
	int decrypted_length = 0;
	int output_length = 0;

	while(std::getline(file, line)){
		input_str += line;
	}
	file.close();
	
	cypher = Base_64_to_Bytes(input_str);	
	decryptedText.resize(cypher.length() + EVP_CIPHER_block_size(EVP_aes_128_ecb()), '\0');


	OpenSSL_add_all_algorithms();
	
	EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
	EVP_DecryptInit_ex(ctx, EVP_aes_128_ecb(), nullptr, (const unsigned char*)key.c_str(), nullptr);
	
	
	if (EVP_DecryptUpdate(ctx, (unsigned char*)decryptedText.c_str(), &decrypted_length, (const unsigned char*)cypher.c_str(), cypher.length()) != 1) {
        	std::cerr << "Error during decryption" << std::endl;
		exit(EXIT_FAILURE);
    	}
	
	if (EVP_DecryptFinal_ex(ctx, reinterpret_cast<unsigned char *>(&decryptedText[0] + decrypted_length), &output_length) != 1) {
		std::cerr << "Error finalizing decryption" << std::endl;
		exit(EXIT_FAILURE);
	}
	
	decryptedText.resize(decrypted_length + output_length);
	EVP_CIPHER_CTX_free(ctx);
	
	return decryptedText;
}

int main(int argc, char** argv){
	
	if(argc > 2){
		std::cout<<Decrypt_AES_128_ECB(argv[1],argv[2])<<'\n';
	}

	return 0;
}
