#pragma once

#include <string>
#include <vector>
#include <sstream> 
#include <iostream> 
#include <fstream> 
#include <iterator>

#include "DataStructureDef.h"

//#define ARRAYSIZE(x) \
//  ((sizeof(x) / sizeof(*(x))) / static_cast<size_t>(!(sizeof(x) % sizeof(*(x)))))

class CsvFile
{
public:
	typedef enum DataType { typeLong, typeString, typeInteger } _dataType;
	DataType _defaultRowTemplate[5] = { typeLong, typeString, typeInteger, typeInteger };

protected:
	std::string _filename;
	char		_delimeter;

	CsvFile()
	{
		_filename = "";
		_delimeter = ',';
	}
	CsvFile(std::string filename, char delimeter = ',')
	{
		_filename = filename;
		_delimeter = delimeter;
	}

public:
	virtual bool Open(std::string filename) = 0;
	virtual bool Open() = 0;

	virtual void Close() = 0;
};


class CsvReader : CsvFile
{
public:
	TradesBySymbol	_tradesBySymbol;
	std::ifstream	_fileStream;

	CsvReader()
		: CsvFile()
	{
		_tradesBySymbol.clear();
	}
	CsvReader(std::string filename, char delimeter = ',')
		: CsvFile(filename, delimeter)
	{
		_tradesBySymbol.clear();
	}
	~CsvReader()
	{
		_fileStream.close();
	}

	virtual bool Open(std::string filename)
	{
		_filename = filename;
		return (Open());
	}
	virtual bool Open()
	{
		if (_filename.empty())
			throw;

		_fileStream.open(_filename, std::fstream::in | std::fstream::app);
		return _fileStream.good();
	}
	virtual void Close()
	{
		_fileStream.close();
	}

	int Read()
	{
		if (!_fileStream.good())
			return -1;

		int row_count = 0;
		std::string row;

		auto tokenizer = [=](std::string line) {
			std::shared_ptr<Trade> tradePtr(new Trade());
			std::string field;
			int index = 0;
			std::istringstream ss(line);
			while (ss)
			{
				if (!std::getline(ss, field, _delimeter))
					break;
				try
				{
					switch (index)
					{
					case 0:	// timestamp
						tradePtr->Timestamp = std::stoull(field);
						break;
					case 1:	// symbol
						tradePtr->Symbol = field;
						break;
					case 2:
						tradePtr->Quantity = std::stoi(field);
						break;
					case 3:
						tradePtr->Price = std::stoi(field);
						break;
					}
					++index;
				}
				catch (const std::invalid_argument e)
				{
					std::cout << "Error " << e.what() << ":" << line << std::endl;
					tradePtr.reset();
				}
			}
			return tradePtr;
		};

		while (std::getline(_fileStream, row))
		{
			++row_count;
			//std::cout << row << '\n';

			auto tradePtr = tokenizer(row);
			if (tradePtr != NULL)
			{
				if (auto it = _tradesBySymbol.find(tradePtr->Symbol); it != _tradesBySymbol.end())
				{
					it->second.push_back(tradePtr);
				}
				else
				{
					Trades trades;
					trades.push_back(tradePtr);

					_tradesBySymbol.insert(
						std::pair<std::string, Trades>(tradePtr->Symbol, trades));
				}
			}
		}

		return row_count;
	}

private:



};

class CsvWriter : CsvFile
{
protected:
	std::ofstream _fileStream;

public:

	CsvWriter()
		: CsvFile()
	{
	}
	CsvWriter(std::string filename, char delimeter = ',')
		: CsvFile(filename, delimeter)
	{}
	~CsvWriter()
	{
		Flush();
		Close();
	}

	virtual bool Open(std::string filename)
	{
		_filename = filename;
		return (Open());
	}
	virtual bool Open()
	{
		_fileStream.exceptions(std::ios::failbit | std::ios::badbit);
		_fileStream.open(_filename);

		return _fileStream.good();
	}
	virtual void Close()
	{
		_fileStream.close();
	}

	void Flush()
	{
		_fileStream.flush();
	}
	CsvWriter& operator << (const char *strchr)
	{
		_fileStream << strchr << _delimeter;
		return *this;
	}

	CsvWriter& operator << (const std::string& str)
	{
		_fileStream << str << _delimeter;
		return *this;
	}

	template<typename T>
	CsvWriter& operator << (const T& t)
	{
		_fileStream << t << _delimeter;
		return *this;
	}

	size_t Write(TradesSummary tradeSummary)
	{
		if (tradeSummary.empty())
			return 0;

		for (auto& rec : tradeSummary)
		{
			// <symbol>,<MaxTimeGap>,<Volume>,<WeightedAveragePrice>,<MaxPrice>

			_fileStream
				<< rec->Symbol << _delimeter
				<< rec->MaxTimeGap << _delimeter
				<< rec->Volume << _delimeter
				<< rec->WeightedAveragePrice << _delimeter
				<< rec->MaxPrice << std::endl;
		}
		Flush();

		return tradeSummary.size();
	}

};