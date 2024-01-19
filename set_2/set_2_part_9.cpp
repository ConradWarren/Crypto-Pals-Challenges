#include <iostream>
#include <cctype>
#include <string>
#include <cstring>

void Print_Byte_String(const std::string& byte_string){
	
	for(int i = 0; i<byte_string.size(); i++){
		
		if(std::isalnum(byte_string[i]) || std::isspace(byte_string[i]) || std::ispunct(byte_string[i])){
			std::cout<<byte_string[i];
		}
		if(true){
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

int main(int argc, char** argv){
	
	if(argc > 1){
		std::string padded_byte_string = PKCS_7(argv[1], 20);
		Print_Byte_String(padded_byte_string);
	}

	return 0;
}
