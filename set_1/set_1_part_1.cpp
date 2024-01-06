#include <iostream>
#include <string>


char Base_64_Char(int num){
	
	num &= 0x3f;
	if(num < 26) return 'A' + num;
	
	if(num < 52) return 'a' + num - 26;

	if(num < 62) return '0' + num - 52;

	if(num == 62) return '+';
	
	return '/';
}

std::string Hex_to_Base_64(const std::string& hex_string){
	
	int bits_counted = 0;
	int current_num = 0;
	std::string result((hex_string.size()%3) ? (hex_string.size()*2)/3 + 1 : (hex_string.size()*2)/3, 'A');
	int idx = result.size()-1;
	for(int i = hex_string.size()-1; i >= 0; i--){
		
		current_num |= (((hex_string[i] >= '0' && hex_string[i] <= '9') ? hex_string[i] - '0' : hex_string[i] - 'a' + 10) << bits_counted);
		bits_counted += 4;
		
		if(bits_counted >= 6){
			result[idx] = Base_64_Char(current_num);
			idx--;
			current_num >>= 6;
			bits_counted -= 6;
		}
	}
	
	if(idx >= 0) result[idx] = Base_64_Char(current_num);	
		
	return result;
}

int main(int argc, char** argv){
	
	if(argc > 1) std::cout<<Hex_to_Base_64(argv[1])<<'\n'; 

	return 0;
}
