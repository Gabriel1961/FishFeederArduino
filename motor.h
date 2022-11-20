#pragma once 

struct Motor
{
  private:
  uint8_t m_in1=0,m_in2=0,m_pwm=0;
  int m_dir = 1; // can be 1 or -1 or 0 to stop
  uint8_t m_speed = 100;
  public:
  
  Motor()
  {

  }

  Motor(uint8_t in1, uint8_t in2, uint8_t pwm)
    : m_in1(in1),m_in2(in2),m_pwm(pwm)
  { 
    pinMode(in1,OUTPUT);
    pinMode(in2,OUTPUT);
    pinMode(pwm,OUTPUT);
    digitalWrite(pwm,HIGH);
    setDir(0);
  }

  // can be 1 or -1
  int getDir()
  {
    return m_dir;
  }
  
  void invertDir()
  {
    setDir(-getDir()); 
  }

  void setDir(int val)
  {
    if(val == 1)
    {
      m_dir = val;
      digitalWrite(m_in1,LOW);
      digitalWrite(m_in2,HIGH);
    }
    else if(val == -1)
    {
      m_dir = val;
      digitalWrite(m_in1,HIGH);
      digitalWrite(m_in2,LOW);
    }
    else if(val == 0)
    {
      m_dir = val;
      digitalWrite(m_in1,LOW);
      digitalWrite(m_in2,LOW);
    }
  }
  
  void setSpeed(uint8_t val)
  {
    m_speed = val; 
    analogWrite(m_pwm,val); 
  }
  uint8_t getSpeed()
  {
    return m_speed;
  }
};
