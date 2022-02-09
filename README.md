# Boolean_search_cpp
Булев поиск по дампу lenta.ru на c++

Инструкция по запуску:

1). программа собирается с помощью приложенного Makefile

2). Программа написана под ос ubuntu 21.04. Для корректной работы требуется библиотека zlib (устанавливается командой $ sudo apt install zlib1g-dev). 

Дополнительные заголовочные библиотеки (для конвертации utf-8 в utf-16) находятся в файлах gzstream.h, checked.h, unchecked.h, core.h, utf8.h. 

Встроены в проект через MakeFile.

3). Программа работает с файлами формата .gz, которые должны:
- находиться в директории программы
- называться dump1.gz, dump2.gz, ..., dump8.gz.

Описание файлов и логики программы:

1). Вспомогательные функции и основной скрипт - в main.cpp, сбор - make, запуск собранной программы: ./main4

2). Основной скрипт:

* считывает данные из .gz файлов, переводит их в utf-16
* строит прямой и обратный индексы в формате std::map<unsigned int, std::set<unsigned int>>
* сжимает их с помощью varbyte в файлы rev_ind.txt, index.txt
* декодирует эти файлы
* принимает запрос от пользователя (вводится с клавиатуры), переводит его в обратную польскую нотацию
* строит по ней дерево
* осуществляет потоковую обработку запроса, выводит результаты
* чтобы прекратить исполнение программы, введите 0.
  
3). Вспомогательные функции:
void first_parsing(std::wstring unparsed, std::list<std::wstring> &parsed) - токенизирует поступивший запрос
  
void polish_notation(std::list<std::wstring> &parsed, std::list<std::wstring> &pol_not) - переводит токенизированный запрос в обратную польскую нотацию
  
void create_node(int &key, int elem_type, int elem_code, std::list<int> &stack, std::map<int, std::list<int>> &tree) - создает вершину дерева запроса
  
int build_tree(std::map<std::wstring, unsigned int> &mp, std::list<std::wstring> &pol_not, std::map<int, std::list<int>> &tree) - строит дерево запроса
  
int srch(int key, int doc_num, std::map<unsigned int, std::set<unsigned int>> &rev_ind, std::map<int, std::list<int>> &tree, int ms) - рекурсивно обходит дерево запроса
  
void process_stream_search(std::list<int> &res, std::map<std::wstring, unsigned int> &mp,
  
                                                std::list<std::wstring> &pol_not, 
  
                                                std::map<unsigned int, 
                                                                  
                                                std::set<unsigned int>> &rev_ind, int ms) - осуществляет потоковый поиск
  
void vb1(unsigned int num, std::ofstream &out) - кодирует одно число в varbyte
  
unsigned int vb1_symb_dec(std::ifstream &in) - декодирует одно число из varbyte
  
void code_dict(std::ofstream &out, std::map<unsigned int, std::set<unsigned int>> &mp) - кодирует индекс в varbyte
  
void decode_dict(std::ifstream &in, std::map<unsigned int, std::set<unsigned int>> &mp) - декодирует индекс из varbyte









