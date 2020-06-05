package SP20_simulator;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.IOException;
import java.util.ArrayList;

public class SicLoader {		// 프로그램을 해석해서 메모리에 올리는 역할을 수행
	ResourceManager rMgr;
	SymbolTable symTab;
	ArrayList<String> title;
	String startAddr;
	ArrayList<Integer> forLength;
	String eStartAddr; 
	int mStartAddr;
	
	public SicLoader(ResourceManager resourceManager) {			//생성자
		// 필요하다면 초기화
		symTab = new SymbolTable();
		title = new ArrayList<String>();
		startAddr = new String();
		eStartAddr = new String();
		forLength = new ArrayList<Integer>();
		mStartAddr = 0;
		setResourceManager(resourceManager);
	}

	/**
	 * Loader와 프로그램을 적재할 메모리를 연결시킨다.
	 * @param rMgr
	 */
	public void setResourceManager(ResourceManager resourceManager) {
		this.rMgr=resourceManager;
	}
	
	public void load(File objectCode){			//object code를 읽어서 load과정을 수행
		int start;
		int pc = 0;
		int length = 0;
		int forCount = 0;
		int csLength = 0;
		int mLength = 0;
		int flag = 0;
		int hFlag =0;
		int tmpNum = 0;
		ArrayList<String> tempM = new ArrayList<String>();
		try {
			FileReader filereader = new FileReader(objectCode);				//파일을 읽는다
			BufferedReader bufReader = new BufferedReader(filereader);
			String line = "";
			while((line = bufReader.readLine()) != null) {
				if(!line.isEmpty() && line.charAt(0) == 'H') {
					String[] sp = line.split(" ");
					title.add(sp[0].substring(1,5));
					start = Integer.parseInt(sp[1].substring(0,6), 16);
					if(hFlag == 0) {
						startAddr = sp[1].substring(0,6);
						mStartAddr = Integer.parseInt(startAddr);
						hFlag = 1;
					}
					if(csLength == 0){
						pc = csLength * 2;
					}else
						pc = csLength * 2;
					symTab.putSymbol(sp[0].substring(1), csLength);
					tmpNum += Integer.parseInt(line.substring(14),16);
					forLength.add(Integer.parseInt(line.substring(14),16));
				}
				else if(!line.isEmpty() && line.charAt(0) == 'D') {
					forCount = 1;
					for(int i = 0 ; i < line.length(); i++) {
						if(Character.isDigit(line.charAt(i))) {
							symTab.putSymbol(line.substring(forCount, i), Integer.parseInt(line.substring(i, i+6),16));
							
							if(i+6 > line.length()) {
								break;
							}
							forCount = i+6;
							i += 6;
							
						}
					}
					
				}
				else if(!line.isEmpty() && line.charAt(0) == 'T') {
					if(pc != Integer.parseInt(line.substring(1,7),16)*2 + csLength*2) {
						pc = Integer.parseInt(line.substring(1,7),16)*2 + csLength*2;
					}
					length = Integer.parseInt(line.substring(7,9), 16);
					for(int i = 9; i < line.length(); i++) {
						rMgr.memory[pc] = line.charAt(i);
						pc++;
		
					}
					
				}
				else if(!line.isEmpty() && line.charAt(0) == 'M') {
					tempM.add(line+" "+Integer.toString(mLength));
				}
				else if(!line.isEmpty() && line.charAt(0) == 'E') {
					if(flag == 0) {
						eStartAddr = line.substring(1,7);
						flag = 1;
					}
					csLength = tmpNum;
					mLength = csLength;
				}
			}
			for(int i = 0; i < tempM.size(); i++) {	//modify계산
				int modifyLength = 0;	//수정해야할 주소
				int change = 0; 		//길이
				int sum;
				String hex = new String();
				String[] split = tempM.get(i).split(" ");
				String operator = tempM.get(i).substring(9,10);
				String temp= new String();
				String operand = split[0].substring(10);
				
				change = Integer.parseInt(tempM.get(i).substring(7,9),16);
				if(change % 2 == 1) {
					if(Integer.parseInt(split[1]) == 0)
						modifyLength = (Integer.parseInt(tempM.get(i).substring(1,7),16) + Integer.parseInt(split[1]))*2 +1;
					else
						modifyLength = (Integer.parseInt(tempM.get(i).substring(1,7),16) + Integer.parseInt(split[1]))*2 +1 ;
					
				}
				else {
					modifyLength = (Integer.parseInt(tempM.get(i).substring(1,7),16) + Integer.parseInt(split[1]))*2;
				}
				for(int j = 0; j < change; j++) {
					if(j == 0) 
						temp = String.valueOf(rMgr.memory[modifyLength+j]);
					else
						temp += rMgr.memory[modifyLength+j];
				}
				sum = symTab.search(operand);
				hex = Integer.toHexString(sum).toUpperCase();
				if(operator.equals("+")) {
					for(int j = 0; j < hex.length(); j++) {
						rMgr.memory[modifyLength+change-j-1] = hex.charAt(hex.length()-j-1);
					}
				}
				else {
					int tempInt = Integer.parseInt(temp,16);
					tempInt -= Integer.parseInt(hex,16);
					hex = Integer.toHexString(tempInt).toUpperCase();
					for(int j = 0; j < hex.length(); j++) {
						rMgr.memory[modifyLength+change-j-1] = hex.charAt(hex.length()-j-1);
					}
				}
			}
			
			filereader.close();
		} catch (FileNotFoundException e) {
			e.printStackTrace();
		} catch(IOException ie) {
			System.out.println(ie);
		}
		
	};
	
	

}
