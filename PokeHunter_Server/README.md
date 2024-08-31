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

|접미사|설명|
|---|---|
|~Manager|여러 스레드에서 **동시 접근 가능** 객체|
|~Worker|스레드 1개가 전담(그러나 Data Race가 없다는 것은 아님)|


<br/>

# 참고
[모두의 코드, 씹어먹는 C++ - <22. 구글에서는 C++ 을 어떻게 쓰는가?>](https://modoocode.com/335)  
