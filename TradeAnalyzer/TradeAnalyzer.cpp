// TradeAnalyzer.cpp : Defines the entry point for the console application.
//

#include <iostream>
#include <string>

#include "stdafx.h"
#include "CsvFileIO.h"
#include "DataStructureDef.h"
#include "AnalysisEngine.h"

#define CATCH_CONFIG_MAIN
#include "catch.hpp"


//TEST_CASE("Test Case 1")
//{
//	REQUIRE(1 == 1);
//}
//TEST_CASE("Test Case 2")
//{
//	REQUIRE(1 == 2);
//}

TEST_CASE(" 1. Open CSV File 'input.csv'.")
{
	CsvReader csvReader;
	SECTION("Analyze Trades read from input file")
	{
		std::string input_file_name("..\\input.csv");
		auto status = csvReader.Open(input_file_name);
		REQUIRE(status == true);

		int rows = csvReader.Read();
		//std::cout << "CSV file read - total rows = " << rows << std::endl;
		REQUIRE(rows == 20438);
		//REQUIRE(rows == 20440);

		csvReader.Close();
	}
}

TEST_CASE("2. Trade Analysis")
{
	SECTION("Analyze Trades read from input file")
	{
		TradesBySymbol tradesBySymbol;

		Trades tradesA;
		std::shared_ptr<TradeRec> rec1(new TradeRec(51300032327, "AAA", 100, 110));
		tradesA.push_back(rec1);

		Trades tradesB;
		std::shared_ptr<TradeRec> rec2(new TradeRec(51300034611, "BBB", 110, 38));
		std::shared_ptr<TradeRec> rec3(new TradeRec(51300069855, "BBB", 5, 100));
		std::shared_ptr<TradeRec> rec4(new TradeRec(51300102708, "BBB", 200, 50));
		std::shared_ptr<TradeRec> rec5(new TradeRec(51300103029, "BBB", 30, 150));
		tradesB.push_back(rec4);
		tradesB.push_back(rec3);
		tradesB.push_back(rec5);
		tradesB.push_back(rec2);

		tradesBySymbol.insert(std::pair(std::string("AAA"), tradesA));
		tradesBySymbol.insert(std::pair(std::string("BBB"), tradesB));

		REQUIRE(tradesBySymbol.size() == 2);

		AnalysisEngine& analysisEngine = AnalysisEngine::Instance();
		auto sumarry = analysisEngine.Analyze(tradesBySymbol);

		REQUIRE(sumarry.size() == 2);

		REQUIRE(sumarry[0]->MaxTimeGap == 0);
		REQUIRE(sumarry[1]->MaxTimeGap > 0);
	}
}

TEST_CASE("3. Output Summary to CSV")
{
	CsvWriter csvWriter;
	SECTION("Write trade summary to output file")
	{
		TradesSummary tradesSummary;

		// <symbol>,<MaxTimeGap>,<Volume>,<WeightedAveragePrice>,<MaxPrice>
		std::shared_ptr<TradeSummaryRec> s1(new TradeSummaryRec("AAA", 100, 1000, 25, 10));
		std::shared_ptr<TradeSummaryRec> s2(new TradeSummaryRec("BBB", 0, 100, 50, 8));
		std::shared_ptr<TradeSummaryRec> s3(new TradeSummaryRec("CCC", 50, 500, 15, 12));

		tradesSummary.push_back(s1);
		tradesSummary.push_back(s2);
		tradesSummary.push_back(s3);

		REQUIRE(tradesSummary.size() == 3);

		std::string output_file_name("..\\output.csv");
		auto status = csvWriter.Open(output_file_name);
		REQUIRE(status == true);

		csvWriter.Write(tradesSummary);
		//csvWriter.Close();
	}
}

TEST_CASE("4. Complete Process Testing")
{
	// Read from input CSV
	CsvReader csvReader;
	std::string input_file_name("..\\input.csv");
	auto status1 = csvReader.Open(input_file_name);
	REQUIRE(status1 == true);

	int rows = csvReader.Read();
	//std::cout << "CSV file read - total rows = " << rows << std::endl;
	REQUIRE(rows == 20438);
	csvReader.Close();

	// Perform Analysis	
	AnalysisEngine& analysisEngine = AnalysisEngine::Instance();
	auto sumarry = analysisEngine.Analyze(csvReader._tradesBySymbol);
	REQUIRE(sumarry.size() == 343);

	// Write result to output CSV
	CsvWriter csvWriter;
	std::string output_file_name("..\\output.csv");
	auto status2 = csvWriter.Open(output_file_name);
	REQUIRE(status2 == true);

	csvWriter.Write(sumarry);
	//csvWriter.Close();	
}

