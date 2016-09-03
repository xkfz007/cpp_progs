#ifdef _TRIE_TREE
/*
a simple implementation of trie tree
Trie树，又称为单词查找树、字典树，是一种树形结构，是一种哈希树的变种，是一种用于快速检索的多叉树数据结构。
*/

const int num_chars = 26;

class Trie {
public:
	Trie();
//	Trie(Trie& tr);
//	virtual ~Trie();
	int trie_search(const char* word, char* entry ) const;
	int insert(const char* word, const char* entry);
//	int remove(const char* word, char* entry);
protected:
	struct Trie_node {
		char* data;
		Trie_node* branch[num_chars];
  		Trie_node();
//		~Trie_node();
	};
	Trie_node* root;
};

Trie::Trie_node::Trie_node() {
	data = NULL;
	for (int i=0; i<num_chars; ++i) branch[i] = NULL;
}

Trie::Trie():root(NULL) {}

int Trie::trie_search(const char* word, char* entry ) const {
	char char_code;
	Trie_node *location = root;
	while( location!=NULL && *word!=0 ) {
		if (*word>='A' && *word<='Z') char_code = *word-'A';
		else if (*word>='a' && *word<='z') char_code = *word-'a';
		else return 0;// 不合法的单词 
		location = location->branch[char_code];
		word++;
	}
	if ( location != NULL && location->data != NULL ) {
		strcpy(entry,location->data);
		return 1;
 	}
 	else return 0;// 不合法的单词
}

int Trie::insert(const char* word, const char* entry) {
	int result = 1;
	if ( root == NULL ) root = new Trie_node;
	char char_code;

	Trie_node *location = root;
	while( location!=NULL && *word!=0 ) {
		if (*word>='A' && *word<='Z') char_code = *word-'A';
		else if (*word>='a' && *word<='z') char_code = *word-'a';
		else return 0;// 不合法的单词 
		if( location->branch[char_code] == NULL ) {
			location->branch[char_code] = new Trie_node;
  		}
		location = location->branch[char_code];
		word++;
	}
	if (location->data != NULL) result = 0;//欲插入的单词已经存在 
	else {
		location->data = new char[strlen(entry)+1];
		strcpy(location->data, entry);
 	}
  	return result;	
}


int main()
{
	Trie t;
	char entry[100];
	t.insert("a", "DET");			t.insert("abacus","NOUN");
	t.insert("abalone","NOUN");		t.insert("abandon","VERB");
	t.insert("abandoned","ADJ");	t.insert("abashed","ADJ");
	t.insert("abate","VERB");		t.insert("this", "PRON");
	if (t.trie_search("this", entry))
		cout <<"'this' was found. pos: " << entry << endl;
	if (t.trie_search("abate", entry))
		cout <<"'abate' is found. pos: " << entry << endl;
	if (t.trie_search("baby", entry))
		cout <<"'baby' is found. pos: " << entry << endl;
	else 
		cout <<"'baby' does not exist at all!" << endl;
}

#endif
