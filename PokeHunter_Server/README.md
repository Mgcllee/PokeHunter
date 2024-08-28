# Develop Branch
개발 전용 브랜치로 **main 브랜치** 와 병합 전 최종 확인 브랜치

<br/>

# Naming Rules
Base: [Google C++ Style Guide](https://google.github.io/styleguide/cppguide.html)  

|Type|Rule|
|---|---|
|Class, Function| CamelCase|
|Variable, Class Field| snake_case|
|enum Field| KEBAB_CASE|

```c++
enum COMPUTER_TYPE { NOTE_BOOK, DESKTOP };

class A {
  void FunctionName();

  int my_age;
}
```

<br/>

# 참고
[모두의 코드, 씹어먹는 C++ - <22. 구글에서는 C++ 을 어떻게 쓰는가?>](https://modoocode.com/335)  
