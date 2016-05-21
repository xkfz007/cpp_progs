
#ifdef _STR_INT
/*整数/浮点等转化为字符串
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
 * 从字符串得到整数/浮点数
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
 * 去除字符串首尾的空格
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
