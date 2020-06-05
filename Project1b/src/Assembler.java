import java.io.*;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.Map;
import java.util.StringTokenizer;


public class Assembler {
	InstTable instTable;  					 // instruction 명세를 저장한 공간 
	ArrayList<String> lineList; 			// 읽어들인 input 파일의 내용을 한 줄 씩 저장하는 공간. 
 	ArrayList<SymbolTable> symtabList;		 // 프로그램의 section별로 symbol table을 저장하는 공간
	ArrayList<LiteralTable> literaltabList; // 프로그램의 section별로 literal table을 저장하는 공간
	ArrayList<TokenTable> TokenList; // 프로그램의 section별로 프로그램을 저장하는 공간

	ArrayList<String> codeList;

	public Assembler(String instFile) {
		instTable = new InstTable(instFile);
		lineList = new ArrayList<String>();
		symtabList = new ArrayList<SymbolTable>();
		literaltabList = new ArrayList<LiteralTable>();
		TokenList = new ArrayList<TokenTable>();
		codeList = new ArrayList<String>();
	}

	public static void main(String[] args) {
		Assembler assembler = new Assembler("inst.data");
		assembler.loadInputFile("input.txt");	
		assembler.pass1();

		assembler.printSymbolTable("symtab_20160312");
		assembler.printLiteralTable("literaltab_20160312");
		assembler.pass2();
		assembler.printObjectCode("output_20160312");
	}

	private void loadInputFile(String inputFile) { //inputFile을 읽어들여서 lineList에 저장한다.
		FileReader inputRd = null;
		BufferedReader inputBr = null;
		try {
			inputRd = new FileReader("C:\\eclipse\\java\\Project1b\\src\\"+inputFile);
			inputBr = new BufferedReader(inputRd);
			String tempLine = null;
			
			while((tempLine = inputBr.readLine()) != null) {
				lineList.add(tempLine);
			
			}
		} catch(IOException e) {
			System.out.println(e);
		}
	}

	/** 
	 * pass1 과정을 수행한다.
	 *   1) 프로그램 소스를 스캔하여 토큰단위로 분리한 뒤 토큰테이블 생성
	 *   2) label을 symbolTable에 정리
	 */
	private void pass1() {
		int check = 0, tmp = 0;
		int countLtorg = 0, count = 0;
		String temp = new String();	//literal 계산하기 위해 만든 임시 문자열
		SymbolTable tempSymTab = new SymbolTable();
		LiteralTable tempLiteralTab = new LiteralTable();
		TokenTable section = new TokenTable(tempSymTab, tempLiteralTab, instTable);
		for(int i = 0; i < lineList.size(); i++) {
			String split[] = lineList.get(i).split("\t");	//문장을 \t으로 토큰분리
			if(split[0] != null && split[0].equals(".")) {
				section.putToken(lineList.get(i));
				continue;
			}
			else if(split[1] != null && split[1].equals("EXTDEF")) {		//extdef
				if(split[2] !=null) {
					StringTokenizer x = new StringTokenizer(split[2], ",\n");
					while(x.hasMoreTokens()) {
						tempSymTab.putExtdef(x.nextToken()); 
					}
				}
			}
			else if(split[1] != null && split[1].equals("EXTREF")) { //extref
				if(split[2] !=null) {
					StringTokenizer x = new StringTokenizer(split[2], ",\n");
					while(x.hasMoreTokens()) {
						tempSymTab.putExtref(x.nextToken()); 
					}
				}
			}
			
			if(split.length > 1 && split[1].equals("CSECT")) {					//operator자리가 CSECT일 때 다른 섹션
				check = i;
				countLtorg = 0;
				TokenList.add(section);
				symtabList.add(tempSymTab);
				literaltabList.add(tempLiteralTab);
				
				tempSymTab = new SymbolTable();
				tempLiteralTab = new LiteralTable();
				section = new TokenTable(tempSymTab, tempLiteralTab, instTable);
			}	
			
 			section.putToken(lineList.get(i));				//토큰을 추가한다.
 			
 			if(split[1] != null && (split[1].equals("LTORG")||split[1].equals("END"))) {
 				if(tempLiteralTab.literalList.size() > 0) {
 					for(int a = countLtorg; a < tempLiteralTab.literalList.size(); a++) {
 						tmp = tempLiteralTab.locationList.get(a);
 						tempLiteralTab.modifyLiteral(tempLiteralTab.literalList.get(a), TokenTable.tempLocation);
 						TokenTable.tempLocation += tmp;
 					}
 					countLtorg = tempLiteralTab.literalList.size();
 				}
 			}
			if(split[1] != null && instTable.instMap.containsKey(split[1])) {	//다음 주소값을 정함
				TokenTable.tempLocation += instTable.instMap.get(split[1]).format;
			}
			
			if(!split[0].isEmpty()) {		//symbol안에 있는 extdef에 주소 지정
				tempSymTab.putSymbol(split[0],section.getToken(i-check).location);
				for(int c = 0; c < tempSymTab.extdefList.size(); c++) {
					if(tempSymTab.extdefList.get(c).equals(split[0])) {
						tempSymTab.modifyExtdef(split[0], section.getToken(i-check).location);
					}
				}
			}
			if(split[0] != null && split[1] != null && split[0].equals("MAXLEN") &&  split[1].equals("EQU")) {	//extdef를 사용했을 때
 				if(split[2] != null) {
 					count = 0;
 					StringTokenizer z = new StringTokenizer(split[2], "-+\n");
 					while(z.hasMoreTokens()) {
 						temp = z.nextToken();
 						for(int a = 0; a < tempSymTab.extdefList.size();a++) {
 							if(temp.equals(tempSymTab.extdefList.get(a))) {
 								if(count == 0) {
 									tmp = tempSymTab.extdefLocationList.get(a);
 									count = 1;
 									break;
 								}
 								else {
 									if(split[2].indexOf("-") != -1) {		//연산자가 -일때
 										tmp -= tempSymTab.extdefLocationList.get(a);
 									}
 									else if(split[2].indexOf("+") != -1) {	//연산자가 +일때
 										tmp += tempSymTab.extdefLocationList.get(a);
 									}
 									break;
 								}
 								
 							}
 						}			
 					}
						tempSymTab.modifySymbol("MAXLEN", tmp);		//symbol의 주소와
						section.getToken(i-check).location = tmp;	//token의 주소값 변경
 				}
 			}
			if(split[0] != null && split[1] != null && split[0].equals("MAXLEN") &&  split[1].equals("WORD")) {	//외부참조를 사용했을 때
				TokenTable.tempLocation += 3;
			}
			
			
			if(split.length > 2 && !split[2].isEmpty() && split[2].charAt(0) == '=') {				//literal추가
				if( split[2].substring(1,3).equals("X'")){				//=X'' 인 경우
					 StringTokenizer z = new StringTokenizer(split[2],"'");
					 z.nextToken();
					 temp = z.nextToken();
					 tempLiteralTab.putLiteral(temp, temp.length()/2);	//리터럴 추가
					 tempLiteralTab.putLiteralObjectCode(temp, 'X');	//리터럴의 object code추가
				 }
				 else if(split[2].substring(1,3).equals("C'")) {		//=C'' 인 경우
					 StringTokenizer z = new StringTokenizer(split[2],"'");
					 z.nextToken();
					 temp = z.nextToken();
   					 tempLiteralTab.putLiteral(temp, temp.length());	//리터럴 추가
   					 tempLiteralTab.putLiteralObjectCode(temp, 'C');	//리터럴의 object code추가
				 }
			}
			
			if(split.length > 2 && split[1] != null) {
				if(instTable.instMap.get(split[1]) != null && instTable.instMap.get(split[1]).format == 2) {	//2형식일 때
					section.getToken(i-check).nixbpe = '4';		//아무도 속하지 않은 임시 nixbpe 지정
				}
			}
			
			if(section.length < TokenTable.tempLocation) {		//section의 최대 길이
				section.length = TokenTable.tempLocation;
			}
		}
		TokenList.add(section);				//끝날 때 추가
		symtabList.add(tempSymTab);
		literaltabList.add(tempLiteralTab);
	}
	
	/**
	 * 작성된 SymbolTable들을 출력형태에 맞게 출력한다.
	 * @param fileName : 저장되는 파일 이름
	 */
	private void printSymbolTable(String fileName) {
		int tmpLoc;
		FileWriter symFw = null;
		BufferedWriter symBw = null;
		File file = new File(fileName);			//파일 생성

		try {
			symFw = new FileWriter(file,false);
			symBw = new BufferedWriter(symFw);
			
			for(int i = 0; i < symtabList.size(); i++ ) {		//symbolTable의 리스트 개수만큼
				for(int j = 0; j < symtabList.get(i).symbolList.size(); j++) {	//symbol 리스트 개수
					tmpLoc = symtabList.get(i).locationList.get(j);			//symbol의 주소 
					symBw.write(symtabList.get(i).symbolList.get(j) + "\t" + Integer.toHexString(tmpLoc).toUpperCase() + "\n");
					symBw.flush();
				}	
				symBw.write("\n");
				symBw.flush();
			}
		}
		catch(IOException e) {
			System.out.println(e);
		}
	}

	/**
	 * 작성된 LiteralTable들을 출력형태에 맞게 출력한다.
	 * @param fileName : 저장되는 파일 이름
	 */
	private void printLiteralTable(String fileName) {		
		int tmpLoc;
		FileWriter litFw = null;
		BufferedWriter litBw = null;
		File file = new File(fileName);			//파일 생성
		try {
			litFw = new FileWriter(file,false);
			litBw = new BufferedWriter(litFw);
			
			for(int i = 0; i < literaltabList.size(); i++ ) {	//literalTable의 리스트 개수만큼
				for(int j = 0; j < literaltabList.get(i).literalList.size(); j++) {		//literal 리스트의 개수
					tmpLoc = literaltabList.get(i).locationList.get(j);
					litBw.write(literaltabList.get(i).literalList.get(j) + "\t" + Integer.toHexString(tmpLoc).toUpperCase() + "\n");
					litBw.flush();
				}	
				litBw.write("\n");
				litBw.flush();
			}
		}
		catch(IOException e) {
			System.out.println(e);
		}
		
	}

	/**
	 * pass2 과정을 수행한다.
	 *   1) 분석된 내용을 바탕으로 object code를 생성하여 codeList에 저장.
	 */
	private void pass2() {
		for(int i = 0; i < TokenList.size(); i++) {
			for(int j = 0; j < TokenList.get(i).tokenList.size(); j++) {
				TokenList.get(i).makeObjectCode(j);
				if(TokenList.get(i).tokenList.get(j).operator != null) {
					if(TokenList.get(i).tokenList.get(j).operator.equals("LTORG") || TokenList.get(i).tokenList.get(j).operator.equals("END")) {	//LTORG, END일 때 literal의 objectCode 추가
						codeList.add(literaltabList.get(i).objectCode.get(0));
						continue;
					}
					else if(TokenList.get(i).tokenList.get(j).operator.equals("CSECT")) {		//CSECT를 구분하기 위해 null 대신 CSECT추가
						codeList.add("CSECT");
						continue;
					}
				}
					codeList.add(TokenList.get(i).tokenList.get(j).objectCode);
			}
		}
	}
	
	/**
	 * 작성된 codeList를 출력형태에 맞게 출력한다.
	 * @param fileName : 저장되는 파일 이름
	 */
	private void printObjectCode(String fileName) {			
		int count = 0;
		int countOperand = 0;
		int countSection = 0;
		int countByte = 0, cByte = 0;
		String tmp = null;
		String tmp1 = null;
		String objectCode = null;
		String modified = null;
		
		Token tempToken;
		FileWriter objFw = null;
		BufferedWriter objBw = null;
		File file = new File(fileName);

		try {
			objFw = new FileWriter(file,false);
			objBw = new BufferedWriter(objFw);

			for(int i = 0; i < codeList.size(); i++) {		//codeList의 개수만큼
				if(codeList.get(i) != null && (codeList.get(i).equals("CSECT") )) {
					if(countSection == 0) {		//첫번째 section 일 때
						for(int j = 0; j < TokenList.get(countSection).tokenList.size(); j++) {	//tokenList의 개수만큼
							tempToken = TokenList.get(countSection).getToken(j);				//j번째 토큰
							if(tempToken.operator.charAt(0) == '+') {							//4형식이고, 외부참조를 사용했을 때	
								modified = "M";
								tmp = Integer.toHexString(tempToken.location+1);
								for(int a = 0; a < 6-tmp.length(); a++) {
									modified+="0";
								}
								modified += Integer.toHexString(tempToken.location+1);
								modified += "05";
								if(tempToken.operand[0] != null) {
									modified += "+" + tempToken.operand[0];
								}
								objBw.write(modified + "\n");		//1번째
								objBw.flush();
							}
						}
						objBw.write("E000000\n");
						objBw.flush();
					}
					else {						//첫번째 section이 아닐 때
						StringBuffer sb = new StringBuffer(objectCode);
						tmp = Integer.toHexString(cByte).toUpperCase();	//byte의 개수
						if(tmp.length() == 1) {
							sb.insert(7, "0"+tmp);
						}
						else {
							sb.insert(7, tmp);
						}
						objBw.write(sb.toString()+"\n");
						objBw.flush();

						for(int j = 0; j < TokenList.get(countSection).tokenList.size(); j++) {
							tempToken = TokenList.get(countSection).getToken(j);
							if(tempToken.operator != null && tempToken.operator.charAt(0) == '+') {		//4형식이며 외부참조를 사용할 때 
								modified = "M";
								tmp = Integer.toHexString(tempToken.location+1);
								for(int a = 0; a < 6-tmp.length(); a++) {
									modified+="0";
								}
								modified += Integer.toHexString(tempToken.location+1);
								modified += "05";
								if(tempToken.operand[0] != null) {
									modified += "+" + tempToken.operand[0];
								}
								objBw.write(modified+"\n");		//2번째
								objBw.flush();
							}

							if(tempToken.operator != null && tempToken.label != null && 		//Extref를 사용할 때
									tempToken.operator.equals("WORD")&& tempToken.label.equals("MAXLEN")) {

								tmp = Integer.toHexString(tempToken.location+1);
								if(tempToken.operand[0] != null) {
									StringTokenizer st = new StringTokenizer(tempToken.operand[0], "+-\n");
									while(st.hasMoreTokens()) {
										tmp1 = st.nextToken();
										modified = "M";
										for(int v = 0; v < symtabList.get(countSection).extrefList.size(); v++) {
											if(symtabList.get(countSection).extrefList.get(v).equals(tmp1)) {
												for(int a = 0; a < 6-tmp.length(); a++) {
													modified+="0";
												}
												modified += Integer.toHexString(tempToken.location).toUpperCase();
												modified += "06";
												if(countOperand == 0 && tempToken.operand[0] != null) {
													modified += "+" + tmp1;
													countOperand++;
												}
												else {
													if(tempToken.operand[0].indexOf("-") != -1) {	//-일 때
														modified += "-" + tmp1;
													}
													else if(tempToken.operand[0].indexOf("+") != -1) {	//+ 일 때
														modified += "+" + tmp1;
													}
												}
												objBw.write(modified + "\n");		//2번째
												objBw.flush();
												break;
											}
										}
									}
								}
							}
							if(j == TokenList.get(countSection).tokenList.size()-1){
								objBw.write("E\n");
								objBw.flush();
							}
						}
					}
					count = i;
					countSection++;
					countByte = 0;
					objBw.write("\n");
					objBw.flush();
				}
				if(i-count == 0) {			//CSECT를 만나 처음부터 시작할 때
					objBw.write("H"+ TokenList.get(countSection).symTab.symbolList.get(0));
					objBw.flush();
					
					tmp = Integer.toHexString(TokenList.get(countSection).length).toUpperCase();

					objBw.write(" " + "000000");
					objBw.flush();
					for(int b = 0; b < 6-tmp.length(); b++) {		//6자리를 채울만큼 0을 쓴다
						objBw.write("0");
						objBw.flush();
					}
					objBw.write(Integer.toHexString(TokenList.get(countSection).length).toUpperCase()+"\n");
					objBw.flush();


					if(symtabList.get(countSection).extdefList.size() != 0) {	//extdef된 symbol을 적는다
						objBw.write("D");
						objBw.flush();

						for(int j = 0; j < symtabList.get(countSection).extdefList.size(); j++) {
							tmp = symtabList.get(countSection).extdefList.get(j);			
							objBw.write(tmp);
							objBw.flush();
							tmp1 = Integer.toHexString(symtabList.get(countSection).search(tmp));
							for(int b = 0; b < 6-tmp1.length();b++) {
								objBw.write("0");
								objBw.flush();
							}
							objBw.write(tmp1.toUpperCase());
							objBw.flush();
						}
						objBw.write("\n");
					}

					if(symtabList.get(countSection).extrefList.size() != 0) {
						objBw.write("R");
						objBw.flush();
						for(int j = 0; j < symtabList.get(countSection).extrefList.size(); j++) {
							objBw.write(symtabList.get(countSection).extrefList.get(j));
							objBw.flush();
						}
						objBw.write("\n");
					}
				}

				if(codeList.get(i) != "CSECT" && codeList.get(i) != null ) {

					if(countByte == 0){
						objectCode = "T000000";

					}
					cByte = countByte / 2;
					countByte += codeList.get(i).length();
					if(countByte > 61 ) { 
						StringBuffer sb = new StringBuffer(objectCode);
						sb.insert(7, Integer.toHexString(cByte).toUpperCase());
						objBw.write(sb.toString()+ "\n");
						objBw.flush();
						
						objectCode = "T";
						tmp = Integer.toHexString(TokenList.get(countSection).getToken(i-count).location).toUpperCase();
						for(int a = 0; a < 6-tmp.length(); a++) {
							objectCode += "0";
						}
						objectCode += tmp;
						countByte = codeList.get(i).length();
					}
					if(TokenList.get(countSection).getToken(i-count).operator != null && TokenList.get(countSection).getToken(i-count).operator.equals("LTORG")) {
						StringBuffer sb = new StringBuffer(objectCode);
						tmp = Integer.toHexString(cByte).toUpperCase();
						if(tmp.length() == 1) {
							sb.insert(7, "0"+tmp);
						}
						else {
							sb.insert(7, tmp);
						}
						objBw.write(sb.toString()+ "\n");
						objBw.flush();

						objectCode = "T";
						tmp = Integer.toHexString(literaltabList.get(countSection).locationList.get(0)).toUpperCase();
						for(int a = 0; a < 6-tmp.length(); a++) {
							objectCode += "0";
						}
						objectCode += tmp;
						objectCode += codeList.get(i);
						StringBuffer sc = new StringBuffer(objectCode);
						tmp = Integer.toHexString(codeList.get(i).length()/2);	//454F46의 길이

						if(tmp.length() == 1) {
							sc.insert(7, "0"+tmp);								//문자열에 삽입
						}
						else {
							sc.insert(7, tmp);
						}
						objBw.write(sc.toString()+ "\n");
						objBw.flush();

					}
					else if(TokenList.get(countSection).getToken(i-count).operator != null && TokenList.get(countSection).getToken(i-count).operator.equals("END")) {
						objectCode += codeList.get(i);
						countByte += codeList.get(i).length()/2;
						cByte = countByte / 2;
					}
					else if(TokenList.get(countSection).getToken(i-count).label != null && 
							TokenList.get(countSection).getToken(i-count).operator != null &&
							TokenList.get(countSection).getToken(i-count).label.equals("MAXLEN") &&
							TokenList.get(countSection).getToken(i-count).operator.equals("WORD")) {
						objectCode += codeList.get(i);
						cByte = countByte / 2;
					}
					else {
						objectCode += codeList.get(i);
					}

					if(i == codeList.size()-1) {
						StringBuffer sb = new StringBuffer(objectCode);
						tmp = Integer.toHexString(cByte).toUpperCase();
						if(tmp.length() == 1) {
							sb.insert(7, "0"+tmp);
						}
						else {
							sb.insert(7, tmp);
						}
						objBw.write(sb.toString()+ "\n");
						objBw.flush();
						for(int j = 0; j < TokenList.get(countSection).tokenList.size(); j++) {
							tempToken = TokenList.get(countSection).getToken(j);
							if(tempToken.operator != null && tempToken.operator.charAt(0) == '+') {
								modified = "M";
								tmp = Integer.toHexString(tempToken.location+1);
								for(int a = 0; a < 6-tmp.length(); a++) {
									modified+="0";
								}
								modified += tmp.toUpperCase();
								modified += "05";
								if(tempToken.operand[0] != null) {
									modified += "+" + tempToken.operand[0];
								}
								objBw.write(modified + "\n");		//3번째
								objBw.flush();
							}
						}
						objBw.write("E");
						objBw.flush();
					}
				}
			}
		}
		catch (IOException e) {
			System.out.println(e);
		}

	}

}
