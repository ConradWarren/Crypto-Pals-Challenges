#include <iostream>
#include <string>

std::string Decrypt_Single_Byte_Cypher(const std::string& hex_string, char key){
	
	int current_num = 0;
	int bits_read = 0;
	std::string result(hex_string.size()%2 ? hex_string.size()/2 + 1 : hex_string.size()/2, '\0');
	int idx = result.size()-1;

	for(int i = hex_string.length()-1; i >= 0; i--){
		
		current_num |= ((hex_string[i] >= '0' && hex_string[i] <= '9') ? hex_string[i] - '0' : hex_string[i] - 'a' + 10) << bits_read;
		bits_read += 4;

		if(bits_read == 8){
			result[idx] = (char)(current_num ^ key);
			current_num = 0;
			bits_read = 0;
			idx--;
		}
	}

	if(idx == 0){
		result[idx] = (char)(current_num ^ key);	
	} 
	
	return result;
}

int main(int argc, char** argv){
	
	if(argc > 2){
		std::cout<<Decrypt_Single_Byte_Cypher(argv[1], argv[2][0])<<'\n';
	}	

	return 0;
}
