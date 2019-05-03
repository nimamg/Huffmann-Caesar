#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <cmath>
#include <cstdlib>
#include <sstream>
#include <cassert>

using namespace std;

#define ENCODE "encode"
#define COMPRESS "compress_only"
#define DECODE "decode"
#define DECOMPRESS "decompress_only"
#define NEXT_LINE '\n'
#define ZERO_ASCII_CODE 48

struct huffman_tree_node {
	int occurrence;
	unsigned char character;
	string path;
	huffman_tree_node* right;
	huffman_tree_node* left;
};


vector <huffman_tree_node*> create_character_nodes_vector ();
string read_input_file_into_string (string file_name);
void count_character_occurrences (string input_text, vector <huffman_tree_node*> &huffman_tree_elements);
huffman_tree_node* create_huffman_tree (vector <huffman_tree_node*> huffman_tree_elements);
void sort_vector (vector <huffman_tree_node*> &huffman_tree_elements);
void create_codes (huffman_tree_node* &node);
string turn_text_into_huffman_codes (string input_text, vector <huffman_tree_node*> huffman_nodes, int &original_binaryString_length);
vector <string> split_huffman_codes (string huffman_codes);
vector <int> binary_to_decimal_codes (vector <string> codes);
string turn_huffman_codes_into_compressed_text (vector <int> codes);
vector <string> decimal_to_binary_codes (vector <int> codes);
void encrypt_text (string &text, int key);
void decrypt_text (string &text, int key);
string turn_codes_into_a_string (vector <string> codes, int original_binaryString_length);
vector <int> compressed_text_to_decimal_ascii_codes (string compressed_text);
string restore_original_text (string compressed_text, vector <string> codes) ;
string create_output_string (vector <huffman_tree_node*> hoffman_nodes, int binaryString_length, string compressed_text);
void write_output_to_file (string output, string filename);
string split_input (string input, vector <string> &codes, int &length, long long int &hash);
string int_to_str (int input);
string hash_to_str (long long int input);
bool check_hash (string input);
long long int create_hash (string text);


void test_sort_func ();
void test_count_func ();
void test_binary_to_decimal();
void test_decimal_to_binary();
void test_encrypt_func();
void test_decrypt_func();
void test_split_code_func();
void test_create_hash();
void test_int_to_str_func();

int main (int argc, char** argv) {

	string command;
	command = argv[1];
	
	if (command == ENCODE || command == COMPRESS) {
		vector <huffman_tree_node*> huffman_tree_elements;
		huffman_tree_elements = create_character_nodes_vector();

		string input_text;
		input_text = read_input_file_into_string (argv[2]);


		count_character_occurrences (input_text, huffman_tree_elements);

		huffman_tree_node* tree_root;
		tree_root = create_huffman_tree(huffman_tree_elements);
		
		create_codes (tree_root);
		
		string binaryString;
		int original_binaryString_length;
		binaryString = turn_text_into_huffman_codes (input_text, huffman_tree_elements, original_binaryString_length);

		vector <string> splitted_codes;
		splitted_codes = split_huffman_codes (binaryString);
		
		vector <int> char_codes;
		char_codes = binary_to_decimal_codes (splitted_codes);
		
		string compressed_text;
		compressed_text = turn_huffman_codes_into_compressed_text (char_codes);
		
		string output_text;
		output_text = create_output_string (huffman_tree_elements, original_binaryString_length,compressed_text);
        
		if (command == ENCODE) {
			
			int key = atoi(argv[4]);
			
			encrypt_text (output_text, key);
		}
		
		write_output_to_file (output_text, argv[3]);

	}



	if (command == DECOMPRESS || command == DECODE) {
		
		string input_data;
		input_data = read_input_file_into_string (argv[2]);
		
		if (command == DECODE) {
			int key = atoi(argv[4]);
			decrypt_text(input_data,key);
		}

		if (!check_hash(input_data)) {
			cout << "This file is corrupted (or edited)." << endl << "Program terminated due to possible errors." << endl;
			return 0;
		}
		
		string input_text;
		int length;
		vector <string> codes (256);
		long long int original_hash;
		input_text = split_input (input_data, codes, length, original_hash);

		vector <int> compressed_characters;
		compressed_characters = compressed_text_to_decimal_ascii_codes (input_text);
			
		vector <string> compressed_codes;
		compressed_codes = decimal_to_binary_codes (compressed_characters);
			
		string complete_binary_text;
		complete_binary_text = turn_codes_into_a_string (compressed_codes, length);
		
		string original_text;
		original_text = restore_original_text (complete_binary_text, codes);
 	
		write_output_to_file(original_text, argv[3]);

	}
	return 0;
}

string read_input_file_into_string (string file_name) {

	fstream file;
	file.open(file_name.c_str());
	string text;
	getline(file,text,'\0');
	file.close();
	return text;
}

void count_character_occurrences (string input_text, vector <huffman_tree_node*> &huffman_tree_elements) {
	for (int i = 0 ; i < input_text.size(); i++)
		huffman_tree_elements[(int)((unsigned char)input_text[i])]->occurrence++;
}

void sort_vector (vector <huffman_tree_node*> &huffman_tree_elements) {
	bool sorted = false;
	while (!sorted) {
		sorted = true;
		for (int i  = 0 ; i < huffman_tree_elements.size() - 1; i++) {
			if (huffman_tree_elements[i]->occurrence > huffman_tree_elements[i+1]->occurrence) {
				swap(huffman_tree_elements[i], huffman_tree_elements[i+1]);
				sorted = false;
			}
		}
	}
}

void encrypt_text (string &text, int key) {
	for (int i = 0 ; i < text.size(); i++) {
		if (text[i] + key < 256 && text[i] + key >= 0)
				text[i] += key;
		else if (text[i] + key >= 256)
			text[i] = text[i] + key - 256;
		else if (text[i] + key < 0)
			text[i] = text[i] + key + 256;
	}
}

void decrypt_text (string &text, int key) {
	for (int i = 0 ; i < text.size(); i++) {
		if (text[i] - key < 256 && text[i] - key >= 0)
				text[i] -= key;
		else if (text[i] - key >= 256)
			text[i] = text[i] - key - 256;
		else if (text[i] - key < 0)
			text[i] = text[i] - key + 256;
	}
}

void insert_element_into_correct_position (vector <huffman_tree_node*> &huffman_tree_elements, huffman_tree_node* new_element) {
	if (huffman_tree_elements.size() == 0) {
		huffman_tree_elements.push_back(new_element);
		return ;
	}
	for (int i  = 0 ; i < huffman_tree_elements.size() ; i++) {
		if (new_element->occurrence < huffman_tree_elements[i]->occurrence) {
			vector<huffman_tree_node*>::iterator position = huffman_tree_elements.begin() + i;
			huffman_tree_elements.insert(position, new_element);
			return ;
		}
	}
}

huffman_tree_node* create_huffman_tree (vector <huffman_tree_node*> tree_elements) {
	sort_vector (tree_elements);
	while (tree_elements.size() > 1) {
		huffman_tree_node *new_element = new huffman_tree_node;
		new_element->occurrence = tree_elements[0]->occurrence + tree_elements[1]->occurrence;

		new_element->left = new huffman_tree_node;
		new_element->right = new huffman_tree_node;
		new_element->left = tree_elements[0];
		new_element->right = tree_elements[1];
		tree_elements.erase(tree_elements.begin());
		tree_elements.erase(tree_elements.begin());
		tree_elements.push_back (new_element);
		sort_vector (tree_elements);

	}
	return tree_elements[0];
}


vector <huffman_tree_node*> create_character_nodes_vector () {

	vector <huffman_tree_node*> huffman_tree_elements (256);
	for (int i  = 0 ; i < 256  ; i++) {
		huffman_tree_elements[i] = new huffman_tree_node;
		huffman_tree_elements[i]->character = (unsigned char)i;
		huffman_tree_elements[i]->left = huffman_tree_elements[i]->right = NULL;
		huffman_tree_elements[i]->path = "";
		huffman_tree_elements[i]->occurrence = 0;
	}
	return huffman_tree_elements;
}

void create_codes (huffman_tree_node* &node) {

	if (node->left != NULL) {
		node->left->path = node->path + '0';
		create_codes (node->left);
	}

	if (node->right != NULL) {
		node->right->path = node->path + '1';
		create_codes (node->right);
	}

}

string turn_text_into_huffman_codes (string input_text, vector <huffman_tree_node*> huffman_nodes, int &original_binaryString_length) {
	string huffman_turned_text = "";
	for (int i = 0 ; i < input_text.size(); i++) {
		huffman_turned_text = huffman_turned_text + huffman_nodes[(unsigned char)input_text[i]]->path;
		
	}
    
    original_binaryString_length = huffman_turned_text.size();
    

	while(huffman_turned_text.size() % 8) {
		huffman_turned_text = huffman_turned_text + '0';
	}

	return huffman_turned_text;
}

vector <string> split_huffman_codes (string huffman_codes) {

	vector <string> splitted_codes;
	for (int i = 0 ; i < huffman_codes.size(); i = i + 8)
		splitted_codes.push_back(huffman_codes.substr(i,8));
	return splitted_codes;
}

vector <int> binary_to_decimal_codes (vector <string> codes) {

	vector <int> new_chars (codes.size() , 0);

	for (int i = 0 ; i < codes.size() ; i++)
		for (int j = 0 ; j < 8 ; j++)
			if (codes[i][j] == '1')
				new_chars[i] += pow(2,8-j-1);
	return new_chars;
}


string turn_huffman_codes_into_compressed_text (vector <int> codes) {
	string compressed_text = "";
	for (int i = 0 ; i < codes.size() ; i++) {
		char converted_char = codes[i];
		compressed_text = compressed_text + converted_char;
	}
	return compressed_text;

}

vector <int> compressed_text_to_decimal_ascii_codes (string compressed_text) {
	vector <int> compressed_characters;
	for (int i = 0 ; i < compressed_text.size(); i++) {
		compressed_characters.push_back(compressed_text[i]);
		if (compressed_characters[i] < 0)
			compressed_characters[i] += 256;
	}
	return compressed_characters;
}

vector <string> decimal_to_binary_codes (vector <int> codes) {
	vector <string> binary_codes (codes.size());
	for (int i = 0; i < codes.size(); ++i) {
		int current_code = codes[i];
		while (current_code / 2) {
			if (current_code % 2)
				binary_codes[i] = '1' + binary_codes[i];
			else
				binary_codes[i] = '0' + binary_codes[i];
			current_code /= 2;
		}
		binary_codes[i] = '1' + binary_codes[i];
	}
	for (int i = 0 ; i < binary_codes.size() ; i++)
		while (binary_codes[i].size() != 8)
			binary_codes[i] = '0' + binary_codes[i];
	return binary_codes;
}

string turn_codes_into_a_string (vector <string> codes, int original_binaryString_length) {
	string complete_binary_text;
	for (int i = 0 ; i < codes.size() ; i++)
		complete_binary_text = complete_binary_text + codes[i];
    
    complete_binary_text.resize(original_binaryString_length);
    
	return complete_binary_text;
}

string restore_original_text (string compressed_text, vector <string> codes)
{
    
	string original_text;

	while (compressed_text.size())
	 {
	 	
		int j = 0 ;
		bool found = false;
		while (!found)
		{


			for (int i = 0; i < codes.size(); ++i)
			{

				if (compressed_text.substr(0,j) == codes[i])
				{
				    
					original_text = original_text + (char)(i);
					found = true;
					break;
				}

			}
			j++;
		}
		compressed_text = compressed_text.substr(j-1);
	}
	return original_text;
}

string create_output_string (vector <huffman_tree_node*> hoffman_nodes, int binaryString_length, string compressed_text) {
	string output_string = "";

	for (int i = 0 ; i < hoffman_nodes.size() ; i++) {
		
		output_string = output_string + hoffman_nodes[i]->path + " ";
	}
	
	output_string = output_string + int_to_str(binaryString_length) + NEXT_LINE;
	
	output_string = output_string + compressed_text;

	long long int hash = create_hash(output_string);
	output_string = hash_to_str(hash) + NEXT_LINE + output_string;
	
	return output_string;
}

string int_to_str (int input) {
	string output;
	while (input/10) {
		output = (char)((input % 10) + ZERO_ASCII_CODE) + output;
		input /= 10;
	}
	output = (char)((input % 10) + ZERO_ASCII_CODE) + output;
	return output;
}

string hash_to_str (long long int input) {
	string output;
	while (input/10) {
		output = (char)((input % 10) + ZERO_ASCII_CODE) + output;
		input /= 10;
	}
	output = (char)((input % 10) + ZERO_ASCII_CODE) + output;
	return output;
}



void write_output_to_file (string output, string filename) {
	ofstream file;
	file.open(filename.c_str());
	file << output;
	file.close();
}

string split_input (string input, vector <string> &codes, int &length, long long int &hash) {
	stringstream input_stream (input);
	input_stream >> hash;
	for (int i = 0 ; i < codes.size() ; i++)
		input_stream >> codes[i];
	string length_string;
	input_stream >> length_string;
	length = atoi(length_string.c_str());
	string compressed_text;
	char temp;
	while(input_stream.get(temp))
        compressed_text += temp;
	
	compressed_text = compressed_text.substr(1);

	return compressed_text;
}

bool check_hash (string input) {
	stringstream input_stream (input);
	long long int hash;
	input_stream >> hash;
	string text;
	char temp;
	while(input_stream.get(temp))
        text += temp;
	text = text.substr(1);
	if (hash != create_hash(text))
		return false;
	return true;
}

long long int create_hash (string text) {
	long long int hash = 0 ; 
	long long int temp;
	for (int i = 0 ; i < text.size(); i++) {
		(temp = hash * pow(2,6)) + (hash * pow(2,16));
		temp -= hash;
		hash = temp + text[i];
	}
	return hash;
}


void test_count_func () {
	vector <huffman_tree_node*> nodes (256);
	for (int i = 0 ; i < 256 ; i++) {
		nodes[i] = new huffman_tree_node;
		nodes[i]->character = i;
	}
	string test = "aaaaabbbbcccdde";
	count_character_occurrences (test, nodes);
	assert (nodes['a']->occurrence == 5);
	assert (nodes['b']->occurrence == 4);
	assert (nodes['c']->occurrence == 3);
	assert (nodes['d']->occurrence == 2);
	assert (nodes['e']->occurrence == 1);


}

void test_sort_func () {
	vector <huffman_tree_node*> tree_elements;
	huffman_tree_node* a = new huffman_tree_node;
	huffman_tree_node* b = new huffman_tree_node;
	huffman_tree_node* c = new huffman_tree_node;
	huffman_tree_node* d = new huffman_tree_node;
	a->occurrence = 5;
	b->occurrence = 11;
	c->occurrence = 1;
	d->occurrence = 4;
	tree_elements.push_back(a);
	tree_elements.push_back(b); tree_elements.push_back(c); tree_elements.push_back(d);
	sort_vector (tree_elements);
	assert(tree_elements[0]->occurrence == 1);
	assert(tree_elements[1]->occurrence == 4);
	assert(tree_elements[2]->occurrence == 5);
	assert(tree_elements[3]->occurrence == 11);

}

void test_binary_to_decimal() {
	vector <string> binary;
	binary.push_back("00010001"); 
	binary.push_back("00000001"); 
	binary.push_back("01100011"); 
	binary.push_back("00001100");
	binary.push_back("00000011");
	binary.push_back("01001111");
	vector <int> decimal = binary_to_decimal_codes(binary);
	assert(decimal[0] == 17);
	assert(decimal[1] == 1);
	assert(decimal[2] == 99);
	assert(decimal[3] == 12);
	assert(decimal[4] == 3);
	assert(decimal[5] == 79);
}

void test_decimal_to_binary() {
	vector <int> decimal (6);
	decimal[0] = 17;
	decimal[1] = 1;
	decimal[2] = 99;
	decimal[3] = 12;
	decimal[4] = 3;
	decimal[5] = 79;
	vector <string> binary = decimal_to_binary_codes (decimal);
	assert (binary[0] == "00010001");
	assert (binary[1] == "00000001");
	assert (binary[2] == "01100011");
	assert (binary[3] == "00001100");
	assert (binary[4] == "00000011");
	assert (binary[5] == "01001111");
}

void test_encrypt_func() {
	string text = "nima";
	encrypt_text (text, 2);
	assert (text == "pkoc");
}


void test_decrypt_func() {
	string text = "pkoc";
	decrypt_text (text, 2);
	assert (text == "nima");
}

void test_split_code_func() {
	string codes = "0001000100000001011000111100";
	vector <string> splitted_codes = split_huffman_codes (codes);
	assert (splitted_codes[0] == "00010001");
	assert (splitted_codes[1] == "00000001");
	assert (splitted_codes[2] == "01100011");
	assert (splitted_codes[3] == "1100");
}

void test_create_hash () {
	string test = "nima is testing this func";
	assert (create_hash (test) == 7477160675562462033);
}

void test_int_to_str_func () {
	int test = 12;
	string temp =  int_to_str (test);
	assert(temp == "12");
}