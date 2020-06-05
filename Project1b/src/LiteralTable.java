import java.util.ArrayList;

/**
 * literal과 관련된 데이터와 연산을 소유한다.
 * section 별로 하나씩 인스턴스를 할당한다.
 */
public class LiteralTable {
	ArrayList<String> literalList = new ArrayList<String>();	//literal의 리스트
	ArrayList<Integer> locationList = new ArrayList<Integer>();	//literal의 주소
	ArrayList<String> objectCode = new ArrayList<String>();		//literal의 object code
	ArrayList<Integer> byteSize = new ArrayList<Integer>();		//object code의 byte 크기
	ArrayList<Integer> isModified = new ArrayList<Integer>();	//주소값이 정해졌는지 확인하는 리스

	/**
	 * 새로운 Literal을 table에 추가한다.
	 * @param literal : 새로 추가되는 literal의 label
	 * @param location : 해당 literal이 가지는 주소값
	 * 주의 : 만약 중복된 literal이 putLiteral을 통해서 입력된다면 이는 프로그램 코드에 문제가 있음을 나타낸다. 
	 * 매칭되는 주소값의 변경은 modifyLiteral()을 통해서 이루어져야 한다.
	 */
	public void putLiteral(String literal, int location) {
		int check = 1;
		for(int i = 0; i < literalList.size(); i++) {
			if(literalList.size() == 0)
				break;
			if(literalList.get(i) == null)
				break;
			else {
				if(literalList.get(i).equals(literal)){
					check = 0;
					break;
				}
			}
		}
		if(check == 1) {
			literalList.add(literal);
			locationList.add(location);
			objectCode.add(null);
			isModified.add(0);
			byteSize.add(-1);
		}
		else if(check == 0){
			return;
		}
	}
	
	public void putLiteralObjectCode(String literal, char XoC) {	//리터럴의 objectCode를 추가한다
		String tempObject = null;
		int temp;
		char tmp;
		for(int i = 0; i < literalList.size(); i++) {
			if(literalList.size() == 0)
				break;
			if(literalList.get(i) == null)
				break;
			else {
				if(literalList.get(i).equals(literal) && isModified.get(i) != 0){
					break;
				}
				else if(literalList.get(i).equals(literal) && isModified.get(i) == 0) {

					if(XoC == 'X') {
						objectCode.set(i,literal);
						byteSize.set(i,literal.length()/2);
					}
					else if(XoC == 'C') {
						for(int a = 0; a < literal.length(); a++) {
							tmp = literal.charAt(a);
							temp = tmp;
							if(a == 0) {
								tempObject = Integer.toHexString(temp).toUpperCase();
							}
							else {
								tempObject += Integer.toHexString(temp).toUpperCase();
							}
						}
						objectCode.set(i, tempObject);
						byteSize.set(i,literal.length());
					}
					isModified.set(i, 1);
				}
			}
		}
	}
	
	/**
	 * 기존에 존재하는 literal 값에 대해서 가리키는 주소값을 변경한다.
	 * @param literal : 변경을 원하는 literal의 label
	 * @param newLocation : 새로 바꾸고자 하는 주소값
	 */
	public void modifyLiteral(String literal, int newLocation) {
		if(literalList.size() > 0) {
			for(int i = 0; i < literalList.size(); i++) {
				if(literalList.get(i).equals(literal)) {
					locationList.set(i, newLocation);
					break;
				}
			}
		}
		else {
			System.out.println("literal을 수정할 수 없습니다.");
		}
	}
	
	/**
	 * 인자로 전달된 literal이 어떤 주소를 지칭하는지 알려준다. 
	 * @param literal : 검색을 원하는 literal의 label
	 * @return literal이 가지고 있는 주소값. 해당 literal이 없을 경우 -1 리턴
	 */
	public int search(String literal) {
		int address = -1;
		if(literalList.size() > 0) {
			for(int i = 0; i < literalList.size();i++) {
				if(literalList.get(i).equals(literal)) {
					address = locationList.get(i);
					break;
				}
			}
		}
		else {
			System.out.println("literal이 없습니다.");
		}
		return address;
	}
}
