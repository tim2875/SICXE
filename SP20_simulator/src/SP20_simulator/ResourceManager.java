package SP20_simulator;

import java.io.File;
import java.io.FileReader;
import java.io.FileWriter;
import java.io.IOException;
import java.util.ArrayList;
import java.util.HashMap;

public class ResourceManager{		// 컴퓨터의 가상 리소스들을 선언하고 관리하는 클래스
	HashMap<String,Object> deviceManager = new HashMap<String,Object>();
	char[] memory = new char[65536]; //메모리
	int[] register = new int[10];	 //레지스터
	double register_F;				 //F레지스터
	int fFlag = 0;					 //dev 구분
	String usingDev = new String();	 //사용중인 dev이름
	int eFlag = 0;					 //파일의 끝 구분
	ArrayList<String> usingDevList = new ArrayList<String>();	//devName의 List

	SymbolTable symtabList;
	// 이외에도 필요한 변수 선언해서 사용할 것.

	public void initializeResource(){	//메모리, 레지스터등 가상 리소스들을 초기화
		for(int i = 0; i < 65536; i++) {
			memory[i] = ' ';
		}
		for(int a = 0; a < 10; a++) {
			register[a] = 0;
		}
		register_F = 0;
	}

	public void closeDevice() {	//deviceManager가 관리하고 있는 파일 입출력 stream들을 전부 종료
		if(usingDevList.size() == 2) {		//2개일 때
			FileReader fr = (FileReader)deviceManager.get(usingDevList.get(0));
			FileWriter fw = (FileWriter)deviceManager.get(usingDevList.get(1));
			try {
				fr.close();
				fw.close();
			} catch (IOException e) {
				e.printStackTrace();
			}
		}
		else if(usingDevList.size() == 1) {	//1개일 때
			FileReader fr = (FileReader)deviceManager.get(usingDevList.get(0));
			try {
				fr.close();
			} catch (IOException e) {
				e.printStackTrace();
			}
		}
		else {
		}
	}

	public void testDevice(String devName) {		//디바이스를 사용할 수 있는 상황인지 체크: TD
		FileWriter writer;
		FileReader reader;
		File file =new File("C:/eclipse/java/SP20_simulator/src/SP20_simulator/"+devName);
		if(file.exists()){		//파일이 존재한다면 
			if(!deviceManager.containsKey(devName)) {		//이미 해쉬맵 키가 존재하는 경우에는 새로 생성하지 않는다
				usingDevList.add(devName);
				if(fFlag == 0) {			//아무것도 생성되어있지 않을 때
					try {
						deviceManager.put(devName, reader = new FileReader(file));
					} catch (IOException e) {
						e.printStackTrace();
					}
					fFlag = 1;
				}
				else if(fFlag == 1) {		//하나만 생성 되어있을 때
					try {
						deviceManager.put(devName, writer = new FileWriter(file));
					} catch (IOException e) {
						e.printStackTrace();
					}
					fFlag = 2;
				}
			}
			else{
			}
			setRegister(9,1);		//SW레지스터를 변경
		}
		else {
			setRegister(9,0);
		}
	}

	public char[] readDevice(String devName, int num){		//디바이스로부터 원하는 개수만큼의 글자를 읽어들인다 : RD
		char[] rst = new char[num];
		usingDev = devName;
		FileReader fr = (FileReader) deviceManager.get(devName);
		try {
			fr.read(rst);
		} catch (IOException e) {
			e.printStackTrace();
		}
		return rst;
	}

	public void writeDevice(String devName, char[] data, int num){	//디바이스로 원하는 개수 만큼의 글자를 출력 : WD
		usingDev = devName;
		FileWriter fw = (FileWriter) deviceManager.get(devName);
		try {
			fw.write(data);
			fw.flush();
		} catch (IOException e) {
			e.printStackTrace();
		}
	}
	
	public char[] getMemory(int location, int num){	//메모리의 특정 위치에서 원하는 개수만큼의 글자를 가져온다.
		char[] getM = new char[num];

		for(int i = 0; i < num; i++) {
			getM[i] = memory[location +i];
		}
		return getM;
	}
	
	public void setMemory(int locate, char[] data, int num){	//메모리의 특정 위치에 원하는 개수만큼의 데이터를 저장한다. 
		for(int i = 0; i < num; i++) {
			memory[locate+i] = data[i];
		}
	}
	
	public int getRegister(int regNum){		//번호에 해당하는 레지스터가 현재 들고 있는 값을 리턴
		int num = register[regNum];
		return num;
	}
	
	public void setRegister(int regNum, int value){	//번호에 해당하는 레지스터에 새로운 값을 입력
		register[regNum] = value;
	}
	
	public char[] intToChar(int data){		//주로 레지스터와 메모리간의 데이터 교환에서 사용
		String temp;
		char[] getD;
		temp = Integer.toString(data);
		getD = temp.toCharArray();
		return getD;
	}
	
	public int charToInt(char[] data){	//주로 레지스터와 메모리간의 데이터 교환에서 사용된다.
		int num;
		String temp;
		temp = String.valueOf(data);
		num = Integer.parseInt(temp,16);
		return num;
	}
}