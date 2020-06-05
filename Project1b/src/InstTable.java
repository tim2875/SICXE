import java.io.*;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.StringTokenizer;

/**
 * 모든 instruction의 정보를 관리하는 클래스. instruction data들을 저장한다
 * 또한 instruction 관련 연산, 예를 들면 목록을 구축하는 함수, 관련 정보를 제공하는 함수 등을 제공 한다.
 */
public class InstTable { // inst.data 파일을 불러와 저장하는 공간.
	HashMap<String, Instruction> instMap;	//key = 명령어, value = Instruction
	ArrayList<String> instLine = new ArrayList<String>();	//파일로부터 가져온 문장
	
	/*
	 * 클래스 초기화. 파싱을 동시에 처리한다.
	 * @param instFile : instruction에 대한 명세가 저장된 파일 이름
	 */
	public InstTable(String instFile) {
		instMap = new HashMap<String, Instruction>();
		openFile(instFile);
	}
	
	/**
	 * 입력받은 이름의 파일을 열고 해당 내용을 파싱하여 instMap에 저장한다.
	 */
	public void openFile(String fileName) {
		FileReader instRd = null ;
		BufferedReader instBr = null;
		Instruction inst = null;
		
		try {
			instRd = new FileReader("C:\\eclipse\\java\\Project1b\\src\\"+fileName);
			instBr = new BufferedReader(instRd);
			String tempLine = null;

			while(( tempLine =  instBr.readLine()) != null ){
				instLine.add(tempLine);
			}
			
			for(int i = 0; i < instLine.size();i++) {
				inst = new Instruction(instLine.get(i));			//??
				instMap.put(inst.instName, inst);
			}
		}
		catch(IOException e){
			System.out.println(e);
		}
	}
}
/**
 * 명령어 하나하나의 구체적인 정보는 Instruction클래스에 담긴다.
 * instruction과 관련된 정보들을 저장하고 기초적인 연산을 수행한다.
 */
class Instruction {
	
	String instName;
	int opcode;				//parseInt()   <-> toHexString
	int numberOfOperand;	//operand의 개수
	int format;				 // instName이 몇 바이트 명령어인지 저장.
	
	
	/**
	 * 클래스를 선언하면서 일반문자열을 즉시 구조에 맞게 파싱한다.
	 * @param line : instruction 명세파일로부터 한줄씩 가져온 문자열
	 */
	public Instruction(String line) {
		parsing(line);
	}
	
	/**
	 * 일반 문자열을 파싱하여 instruction 정보를 파악하고 저장한다.
	 * @param line : instruction 명세파일로부터 한줄씩 가져온 문자열
	 */
	public void parsing(String line) {
		int count = 0;
		StringTokenizer a = new StringTokenizer(line, "\t");
		while(a.hasMoreTokens()) {
			if(count == 0) {			//이름
				instName = a.nextToken();
			}
			else if(count == 1) {		//타입
				StringTokenizer b = new StringTokenizer(a.nextToken(), "/");
				format = Integer.parseInt(b.nextToken());
			}
			else if(count == 2) {		//
				opcode = Integer.parseInt(a.nextToken(),16);
			}
			else if(count == 3) {
				numberOfOperand = Integer.parseInt(a.nextToken());
				count = 0;
			}
			count++;
		}
	}
}