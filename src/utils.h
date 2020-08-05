#include <EEPROM.h>
#include <String.h>

bool EEPROMWriteString(String str, unsigned int posS, unsigned int posE)
{
  for (unsigned int i = 0; i <= posE; i++)
  {
    char t = str[i];
    EEPROM.write(posS + i, t ? t : '\0');
  }
  return EEPROM.commit();
}

String EEPROMReadString(int posS, int posE)
{
  String text = "";
  for (int i = posS; i <= posE; i++)
  {
    char t = (char)EEPROM.read(i);
    if (t != '\0')
      text += t;
  }
  text.trim();
  return text;
}