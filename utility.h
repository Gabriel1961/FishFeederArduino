#pragma once
int getNewJsonIdx(const String& str, int stidx)
{
  int cnt = 0;
  int b = 0;
  for(int i=stidx;i<str.length();i++)
  {
    if(str[i] == '{')
      cnt++,b=1;
    else if (str[i] == '}')
      cnt--,b=1;
    
    if(b && cnt == 0)
      return i+1;
  }
  return str.length();
}
