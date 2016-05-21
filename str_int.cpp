
#ifdef _STR_INT
/*����/�����ת��Ϊ�ַ���
 */
#include <ostream> // for the << operator
#include <sstream> // for ostringstream
#include <string>  // for string

template<class T>
std::string to_string(T const& obj)
{
  std::ostringstream out;
  out << obj;
  return out.str();
}
/*
 * ���ַ����õ�����/������
 * */
#include <istream> // for the >> operator
#include <sstream> // for ostringstream
#include <string>  // for string
template<class T>
T from_string(std::string const& str)
{
  std::istringstream in(str);
  T result;
  if (in >> result)
    return result;
  else
    throw "conversion_error";
}

/*
 * ȥ���ַ�����β�Ŀո�
 * */
std::string& trim(std::string &s)
{
    if (s.empty())
    {
        return s;
    }

    s.erase(0,s.find_first_not_of(" "));
    s.erase(s.find_last_not_of(" ") + 1);
    return s;
}
#endif