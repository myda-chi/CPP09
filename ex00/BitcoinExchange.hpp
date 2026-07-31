#ifndef BITCOINEXCHANGE_HPP
#define BITCOINEXCHANGE_HPP

#include <string>
#include <map>
#include <iostream>
#include <fstream>

class BitcoinExchange
{
	public:
		BitcoinExchange();
		~BitcoinExchange();

		void processInputFile(const std::string &inputFile) const;
	private:
		std::map<std::string, double>	_rates;

		void	loadDatabase(const std::string &databaseFile);
		double	getRate(const std::string &date) const;

		static bool			isValidDate(const std::string &date);
		static bool			isLeapYear(int year);
		static std::string	trim(const std::string &s);
};

#endif
