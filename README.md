# 설명
(대충 긴 김진우 서버)에서 사용할 수 있도록 만든 팰 교배 최적 경로 계산기입니다.
특정한 부모팰 하나에서 얻고자하는 자식팰로 내려가는 최적의 경로를 계산합니다.

# 사용법
### 교배식 작성
교배를 통해 얻어낸 교배식을 "교배식.txt"에 작성합니다.("교배식.txt"는 항상 실행프로그램과 같은 폴더내에 존재해야합니다.)
한 줄에 하나씩 "+", "=", "<띄어쓰기>를 사이에 두고, 부모팰, 부모팰, 자식팰 순으로 기존의 교배식을 작성합니다.
예시)
도로롱+꼬꼬닭=빙천마
도로롱=꼬꼬닭=빙천마
도로롱+꼬꼬닭+빙천마
도로롱 꼬꼬닭 빙천마

### 프로그램 실행
1. PalShortPath.exe를 실행합니다.
2. 교배를 할 팰과 최종적으로 교배를 통해 얻고싶은 팰을 프로그램 설명에 맞춰 각각 입력합니다.
3. 결과를 통해 교배루트를 확인합니다.

# 주의사항
1. 7번 이상의 교배를 통해야 팰을 얻을 수 있는 경우는 계산하지 않습니다.
2. 교배식에 작성된 동일한 팰 이름은 다 통일해주세요. 예시) 캐티, 캐티메이지 혼용 불가
