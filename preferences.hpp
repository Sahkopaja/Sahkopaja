#ifndef _PREFERENCES_HPP_
#define _PREFERENCES_HPP_

#include <jsoncpp/json/json.h>
#include <string>
#include <vector>
#include <exception>
#include <fstream>


//Basic JSON config file reader based on libjsoncpp
class Preferences
{
	private:
		std::string fileName;
		Json::Value root;

	public:
		Preferences(std::string _fileName) : fileName(_fileName)
		{
			Json::Reader reader;

			std::ifstream fs(fileName, std::ios::in);

			if (!fs.is_open())
				throw std::runtime_error("File " + fileName + " couldn't be opened\n");

			bool parseSuccess = reader.parse(fs, root);

            if (fs.is_open())
                fs.close();
		}

		std::string readString(std::string key, std::string def)
		{
			Json::Value val = root.get(key, def);
			if (val.isString())
				return val.asString();
			else
				return def;
		}

		int readInt(std::string key, int def)
		{
			Json::Value val = root.get(key, def);
			if (val.isInt())
				return val.asInt();
			else
				return def;
		}

		double readDouble(std::string key, double def)
		{
			Json::Value val = root.get(key, def);
			if (val.isDouble())
				return val.asDouble();
			else
				return def;
		}
};

#endif
