#include <iostream>
#include <fstream>
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

void Find_Possible_Cyphers(const std::string& input_file_path){

	std::ifstream file(input_file_path);
	std::string line;
	std::string decrypted_cypher;
	int alpha_numeric_character_count = 0;
	
	while(std::getline(file, line)){
		for(int i = 0; i <= 0xff; i++){
			decrypted_cypher = Decrypt_Single_Byte_Cypher(line, (char)i);
			alpha_numeric_character_count = 0;
			for(int j = 0; j<decrypted_cypher.size(); j++){
				if((decrypted_cypher[j] >= 'a' && decrypted_cypher[j] <= 'z') || (decrypted_cypher[j] >= 'A' && decrypted_cypher[j] <= 'Z') || decrypted_cypher[j] == ' '){
					alpha_numeric_character_count++;
				}
			}
			
			if(alpha_numeric_character_count >= decrypted_cypher.size()-1){
				std::cout<<decrypted_cypher<<"\n";
			}
		}
	}
}

int main(int argc, char** argv){
	
	if(argc > 1){
		std::cout<<"starting search\n";
		Find_Possible_Cyphers(argv[1]);
	}
	
	return 0;
}
