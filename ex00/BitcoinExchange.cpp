#include "BitcoinExchange.hpp"

BitcoinExchange::BitcoinExchange()
{
}

BitcoinExchange::~BitcoinExchange()
{
}

std::string BitcoinExchange::trim(const std::string &s)
{
	size_t start = 0;
	size_t end = s.size();
 
	while (start < end && std::isspace(static_cast<unsigned char>(s[start])))
		start++;
	while (end > start && std::isspace(static_cast<unsigned char>(s[end - 1])))
		end--;
	return (s.substr(start, end - start));
}
 
bool BitcoinExchange::isLeapYear(int year)
{
	if (year % 4 != 0)
		return (false);
	if (year % 100 == 0 && year % 400 != 0)
		return (false);
	return (true);
}

bool BitcoinExchange::isValidDate(const std::string &date)
{
	if (date.size() != 10)
		return (false);
	if (date[4] != '-' || date[7] != '-')
		return (false);
	for (size_t i = 0; i < date.size(); i++)
	{
		if (i == 4 || i == 7)
			continue;
		if (!std::isdigit(static_cast<unsigned char>(date[i])))
			return (false);
	}
 
	int year = std::atoi(date.substr(0, 4).c_str());
	int month = std::atoi(date.substr(5, 2).c_str());
	int day = std::atoi(date.substr(8, 2).c_str());
 
	if (month < 1 || month > 12)
		return (false);
	if (day < 1)
		return (false);
 
	static const int daysInMonth[12] =
		{31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
	int maxDay = daysInMonth[month - 1];
	if (month == 2 && isLeapYear(year))
		maxDay = 29;
	if (day > maxDay)
		return (false);
	return (true);
}
void BitcoinExchange::loadDatabase(const std::string &databaseFile)
{
	std::ifstream ifs(databaseFile.c_str());
	if (!ifs.is_open())
		std::cerr << "Error: could not open database file." << std::endl;
 
	std::string line;
	bool firstLine = true;
	while (std::getline(ifs, line))
	{
		if (line.empty())
			continue;
		if (firstLine)
		{
			firstLine = false;
			continue;
		}
		size_t pos = line.find(',');
		if (pos == std::string::npos)
			continue;
 
		std::string date = trim(line.substr(0, pos));
		std::string valueStr = trim(line.substr(pos + 1));
 
		if (!isValidDate(date) || valueStr.empty())
			continue;
 
		char *endptr = NULL;
		double rate = std::strtod(valueStr.c_str(), &endptr);
		if (endptr == valueStr.c_str() || *endptr != '\0')
			continue;
 
		_rates[date] = rate;
	}
 
	if (_rates.empty())
		std::cerr << "Error: database is empty or invalid." << std::endl;
}


void BitcoinExchange::processInputFile(const std::string &inputFile) const
{

}
