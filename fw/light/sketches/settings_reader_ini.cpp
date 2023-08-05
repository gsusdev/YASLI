#include "settings_reader_ini.h"

SettingsReaderIni::~SettingsReaderIni()
{
	close();
}
	
bool SettingsReaderIni::open(const String& filename)
{
	assert(_ini_p == nullptr);
		
	_ini_p = new IniFile(_fileSystem_p, filename.c_str());
	if (!_ini_p->open())
	{
		LogError msg(_logger, ILogger::ErrorSeverity::error);
		
		msg.addText("Failed to open file ");
		msg.addText(std::string(filename.c_str()));		
		
		close();
		return false;
	}
	
	return true;
} 

void SettingsReaderIni::close()
{
	if (_ini_p == nullptr)
	{
		return;
	}
	
	if (_ini_p->isOpen())
	{
		_ini_p->close();
	}
	
	delete _ini_p;
}
	
bool SettingsReaderIni::readGroup(const std::string& iniGroupName, ISettingsGroup& destination)
{
	if (_ini_p == nullptr)
	{
		return false;
	}
	
	const auto itemCnt = destination.getItemCount();
	std::string errorText;
	
	auto ok = true;
		
	for (size_t i = 0; i < itemCnt; ++i)
	{
		const auto& itemName = destination.getItemName(i);
		
		if (_ini_p->getValue(iniGroupName.c_str(), itemName.c_str(), _buf, sizeof(_buf)))
		{
			if (!destination.setItemText(itemName, std::string(_buf), &errorText))
			{
				ok = false;
				
				LogError msg(_logger, ILogger::ErrorSeverity::error);
				
				msg.addText("Parameter \"");
				msg.addText(itemName);
				msg.addText("\": ");
				msg.addText(errorText);
			}
		}
	}
	
	return ok;
}