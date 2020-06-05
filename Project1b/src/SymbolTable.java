import java.util.ArrayList;

/**
 * symbol과 관련된 데이터와 연산을 소유한다.
 * section 별로 하나씩 인스턴스를 할당한다.
 */
public class SymbolTable {
	ArrayList<String> symbolList = new ArrayList<String>();
	ArrayList<Integer> locationList = new ArrayList<Integer>();
	ArrayList<String> extdefList = new ArrayList<String>();
	ArrayList<Integer> extdefLocationList = new ArrayList<Integer>();
	ArrayList<String> extrefList = new ArrayList<String>();

	// 기타 literal, external 선언 및 처리방법을 구현한다.
	
	/**
	 * 새로운 Symbol을 table에 추가한다.
	 * @param symbol : 새로 추가되는 symbol의 label
	 * @param location : 해당 symbol이 가지는 주소값
	 * 주의 : 만약 중복된 symbol이 putSymbol을 통해서 입력된다면 이는 프로그램 코드에 문제가 있음을 나타낸다. 
	 * 매칭되는 주소값의 변경은 modifySymbol()을 통해서 이루어져야 한다.
	 */
	public void putSymbol(String symbol, int location) {
		int check = 1;
		if(symbolList.size() > 1){
			for(int i = 0; i < symbolList.size(); i++) {
				if(symbolList.get(i) == null)
					break;
				else {
					if(symbolList.get(i).equals(symbol)){
						System.out.println("같은 Symbol이 존재합니다.");
						check = 0;
						break;
					}
				}
			}
		}
		if(check == 1) {
			symbolList.add(symbol);
			locationList.add(location);
		}
		
	}
	
	/**
	 * 기존에 존재하는 symbol 값에 대해서 가리키는 주소값을 변경한다.
	 * @param symbol : 변경을 원하는 symbol의 label
	 * @param newLocation : 새로 바꾸고자 하는 주소값
	 */
	public void modifySymbol(String symbol, int newLocation) {
		if(symbolList.size() > 0) {
			for(int i = 0; i < symbolList.size(); i++) {
				if(symbolList.get(i).equals(symbol)) {
					locationList.set(i, newLocation);
					break;
				}
			}
		}
		else {
			System.out.println("symbol을 수정할 수 없습니다.");
		}
	}
	
	/**
	 * 인자로 전달된 symbol이 어떤 주소를 지칭하는지 알려준다. 
	 * @param symbol : 검색을 원하는 symbol의 label
	 * @return symbol이 가지고 있는 주소값. 해당 symbol이 없을 경우 -1 리턴
	 */
	public int search(String symbol) {
		int address = -1;
		if(symbolList.size() > 0) {
			for(int i = 0; i < symbolList.size();i++) {
				if(symbolList.get(i).equals(symbol)) {
					address = locationList.get(i);
					break;
				}
			}
		}
		else {
			System.out.println("symbol이 없습니다.");
		}
		return address;
	}
	
	public void putExtdef(String extdef) {		//Extdef 리스트를 추가하는 함수
		extdefList.add(extdef);
		extdefLocationList.add(-1);
	}
	
	
	public void modifyExtdef(String extdef, int newLocation) {		//Extdef 리스트에 있는 값의 주소를 바꿔주는 함수
		if(extdefList.size() > 0) {
			for(int i = 0; i < extdefList.size(); i++) {
				if(extdefList.get(i).equals(extdef)) {
					extdefLocationList.set(i, newLocation);
					break;
				}
			}
		}
		else {
			System.out.println("extdef를 수정할 수 없습니다.");
		}
	}
	
	public void putExtref(String extref) {			//Extref 리스트를 추가하는 함수
		extrefList.add(extref);
	}
}
