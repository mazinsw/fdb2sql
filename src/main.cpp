#include <iostream>
#include <fstream>
#include <streambuf>
#include <iomanip>
#include <algorithm>
#include <functional>
#include <cctype>
#include <locale>
#include <math.h>
#include <string.h>
#include "ibpp.h"
#define round(x) floor((x)+0.5)
#include <stdarg.h>  // For va_start, etc.

std::string string_format(const std::string fmt, ...)
{
	int size = ((int)fmt.size()) * 2 + 50;   // Use a rubric appropriate for your code
	std::string str;
	va_list ap;
	while (1)       // Maximum two passes on a POSIX system...
	{
		str.resize(size);
		va_start(ap, fmt);
		int n = vsnprintf((char *)str.data(), size, fmt.c_str(), ap);
		va_end(ap);
		if (n > -1 && n < size)    // Everything worked
		{
			str.resize(n);
			return str;
		}
		if (n > -1)  // Needed size returned
			size = n + 1;   // For null char
		else
			size *= 2;      // Guess at a larger size (OS specific)
	}
	return str;
}

// trim from start
static inline std::string &ltrim(std::string &s)
{
	s.erase(s.begin(), std::find_if(s.begin(), s.end(),
	                                std::not1(std::ptr_fun<int, int>(std::isspace))));
	return s;
}

// trim from end
static inline std::string &rtrim(std::string &s)
{
	s.erase(std::find_if(s.rbegin(), s.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(),
	        s.end());
	return s;
}

// trim from both ends
static inline std::string &trim(std::string &s)
{
	return ltrim(rtrim(s));
}

std::string SqlEscapeString(const std::string& str)
{
	std::string result;
	for (std::string::const_iterator i = str.begin(), end = str.end(); i != end; ++i)
	{
		if(*i == '\\' || *i == '\'' || *i == '"')
			result.append(1, '\\');
		result.append(1, *i);
	}
	return result;
}

void split(const std::string& s, char delim, std::vector<std::string>& v)
{
	size_t i = 0;
	size_t pos = s.find(delim);
	while (pos != std::string::npos)
	{
		v.push_back(s.substr(i, pos - i));
		i = ++pos;
		pos = s.find(delim, pos);
	}
	if (i < s.length() - 1)
		v.push_back(s.substr(i, s.length()));
}

int run_export(const std::string & query, IBPP::Statement &st,
               const std::string & table, std::ostream &out)
{
	int scale, plates;
	float floatVal;
	double dblVal;
	int64_t intVal;
	std::string strVal;
	IBPP::Timestamp tmsVal;
	IBPP::Time timeVal;
	IBPP::Date dateVal;

	st->Execute(query);
	if(!st->Fetch())
		return 1;
	int columnCount = st->Columns();
	int first = 1;
	out << "INSERT INTO `" << table << "` (";
	for(int i = 1; i <= columnCount; i++)
	{
		if(i > 1)
			out << ", ";
		out << '`' << st->ColumnName(i) << '`';
	}
	out << ") VALUES" << std::endl;
	do
	{
		if(!first)
			out << ")," << std::endl;
		first = 0;
		out << "\t(";
		for(int i = 1; i <= columnCount; i++)
		{
			IBPP::SDT columnType = st->ColumnType(i);
			if(i > 1)
				out << ", ";
			if(st->IsNull(i))
			{
				out << "NULL";
				continue;
			}
			switch(columnType)
			{
			case IBPP::sdArray:
				out << "NULL"; // no supported
				break;
			case IBPP::sdBlob:
				out << "NULL"; // no supported
				break;
			case IBPP::sdDate:
				st->Get(i, dateVal);
				out << "'" << std::setfill('0') << std::setw(4) << dateVal.Year() << "-";
				out << std::setfill('0') << std::setw(2) << dateVal.Month() << "-";
				out << std::setfill('0') << std::setw(2) << dateVal.Day() << "'";
				break;
			case IBPP::sdTime:
				st->Get(i, timeVal);
				out << "'" << std::setfill('0') << std::setw(2) << timeVal.Hours() << ":";
				out << std::setfill('0') << std::setw(2) << timeVal.Minutes() << ":";
				out << std::setfill('0') << std::setw(2) << timeVal.Seconds() << "'";
				break;
			case IBPP::sdTimestamp:
				st->Get(i, tmsVal);
				out << "'" << std::setfill('0') << std::setw(4) << tmsVal.Year() << "-";
				out << std::setfill('0') << std::setw(2) << tmsVal.Month() << "-";
				out << std::setfill('0') << std::setw(2) << tmsVal.Day() << " ";
				out << std::setfill('0') << std::setw(2) << tmsVal.Hours() << ":";
				out << std::setfill('0') << std::setw(2) << tmsVal.Minutes() << ":";
				out << std::setfill('0') << std::setw(2) << tmsVal.Seconds() << "'";
				break;
			case IBPP::sdString:
				st->Get(i, strVal);
				out << '"' << SqlEscapeString(strVal) << '"';
				break;
			case IBPP::sdFloat:
				st->Get(i, floatVal);
				out << floatVal;
				break;
			case IBPP::sdDouble:
				st->Get(i, dblVal);
				out << dblVal;
				break;
			case IBPP::sdSmallint:
			case IBPP::sdInteger:
			case IBPP::sdLargeint:
				intVal = 0;
				st->Get(i, intVal);
				plates = st->ColumnScale(i);
				scale = (int) round(pow(10, plates));
				if(plates > 0)
					out << (intVal / scale) << "." << std::setfill('0') << std::setw(plates) << (abs(intVal) % scale);
				else
					out << intVal;
				break;
			}
		}
	}
	while (st->Fetch());
	if(!first)
		out << ");" << std::endl;
	out << std::endl;
	return 0;
}

void export_tables(IBPP::Statement &st, std::ostream &out)
{
	std::vector<std::string> tables;
	std::string strVal;
	st->Execute("select rdb$relation_name "
	            "from rdb$relations "
	            "where rdb$view_blr is null "
	            "and (rdb$system_flag is null or rdb$system_flag = 0)");
	while (st->Fetch())
	{
		st->Get(1, strVal);
		tables.push_back(trim(strVal));
	}
	for(int j = 0; j < (int)tables.size(); j++)
	{
		std::string table = tables[j];
		run_export("SELECT * FROM " + table, st, table, out);
	}
}

void export_input(IBPP::Statement &st, const char *input, std::ostream &out)
{
	std::ifstream t(input);
	std::string stmts((std::istreambuf_iterator<char>(t)),
	                  std::istreambuf_iterator<char>());
	std::vector<std::string> stmt_list;
	split(stmts, ';', stmt_list);
	for(int i = 0; i < (int)stmt_list.size(); i++)
	{
		std::string stmt = stmt_list[i];
		run_export(stmt, st, string_format("TABLE_%d", i + 1), out);
	}
}

int show_usage(const char *cmd)
{
	std::cout << "FDB To SQL: Export FDB data to SQL statement" << std::endl;
	std::cout << "Version: 1.0.0.0" << std::endl;
	std::cout << "Invalid command: " << cmd << std::endl;
	std::cout << "Usage: fdb2sql -d database -u user -p password -i input -o output" << std::endl;
	std::cout << "\t-d: full path to fdb file" << std::endl;
	std::cout << "\t-h: hostname, default: localhost" << std::endl;
	std::cout << "\t-u: user to connect, default: SYSDBA" << std::endl;
	std::cout << "\t-p: password to connect, default: masterkey" << std::endl;
	std::cout << "\t-i: path to input sql file to execute, when away export all data "
	          "from all tables" << std::endl;
	std::cout << "\t    split each statement by \";\" and create a insert statement "
	          "with table name TABLE_[1...STMT_COUNT]" << std::endl;
	std::cout << "\t-o: output file to export data into" << std::endl;
	return 1;
}

int main(int argc, char **argv)
{
	const char *_hostname = "localhost", *_username = "SYSDBA", *_password = "masterkey";
	const char *hostname = _hostname, *database = NULL, *username = _username,
	            *password = _password, *input = NULL, *output = NULL;
	int ret_val = 0;
	for(int i = 1; i < argc; i++)
	{
		if(strcmp(argv[i], "-h") == 0 && i < argc - 1)
			hostname = argv[i + 1];
		else if(strcmp(argv[i], "-d") == 0 && i < argc - 1)
			database = argv[i + 1];
		else if(strcmp(argv[i], "-u") == 0 && i < argc - 1)
			username = argv[i + 1];
		else if(strcmp(argv[i], "-p") == 0 && i < argc - 1)
			password = argv[i + 1];
		else if(strcmp(argv[i], "-i") == 0 && i < argc - 1)
			input = argv[i + 1];
		else if(strcmp(argv[i], "-o") == 0 && i < argc - 1)
			output = argv[i + 1];
		else
			return show_usage(argv[i]);
		i++;
	}
	if(database == NULL)
		return show_usage("-d");
	std::ofstream *outf = NULL;
	try
	{
		IBPP::Database db = IBPP::DatabaseFactory(hostname, database, username, password);
		db->Connect();
		IBPP::Transaction tr = IBPP::TransactionFactory(db);
		tr->Start();
		IBPP::Statement st = IBPP::StatementFactory(db, tr);
		std::ostream *outp = &std::cout;
		if(output != NULL)
		{
			outf = new std::ofstream(output);
			outp = outf;
		}
		std::ostream &out = *outp;
		if(input != NULL)
			export_input(st, input, out);
		else
			export_tables(st, out);
		tr->Rollback();
		db->Disconnect();
	}
	catch (IBPP::Exception& e)
	{
		std::cout << e.what();
		ret_val = 1;
	}
	if(outf != NULL)
	{
		outf->close();
		delete outf;
	}
	return ret_val;
}