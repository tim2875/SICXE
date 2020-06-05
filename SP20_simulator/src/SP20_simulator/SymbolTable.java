package SP20_simulator;
import java.util.ArrayList;

public class SymbolTable {	//symbol과 관련된 데이터와 연산을 소유
	ArrayList<String> symbolList = new ArrayList<String>();
	ArrayList<Integer> addressList  = new ArrayList<Integer>();
	ArrayList<String> extdefList = new ArrayList<String>();
	ArrayList<Integer> extdefLocationList = new ArrayList<Integer>();
	
	public void putSymbol(String symbol, int location) {	//새로운 Symbol을 table에 추가한다.
		int check = 1;
		if(symbolList.size() > 1){
			for(int i = 0; i < symbolList.size(); i++) {
				if(symbolList.get(i) == null)
					break;
				else {
					if(symbolList.get(i).equals(symbol)){
						check = 0;
						break;
					}
				}
			}
		}
		if(check == 1) {
			symbolList.add(symbol);
			addressList.add(location);
		}	
	}
	public void modifySymbol(String symbol, int newLocation) {		//기존에 존재하는 symbol 값에 대해서 가리키는 주소값을 변경
		if(symbolList.size() > 0) {
			for(int i = 0; i < symbolList.size(); i++) {
				if(symbolList.get(i).equals(symbol)) {
					addressList.set(i, newLocation);
					break;
				}
			}
		}
		else {
		}
	}

	public int search(String symbol) {		//인자로 전달된 symbol의 주소 반환
		int address = -1;
		if(symbolList.size() > 0) {
			for(int i = 0; i < symbolList.size();i++) {
				if(symbolList.get(i).equals(symbol)) {
					address = addressList.get(i);
					break;
				}
			}
		}
		else {
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
		}
	}

}
