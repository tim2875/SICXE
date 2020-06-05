import java.util.ArrayList;
import java.util.StringTokenizer;

/**
 * 사용자가 작성한 프로그램 코드를 단어별로 분할 한 후, 의미를 분석하고, 최종 코드로 변환하는 과정을 총괄하는 클래스이다. <br>
 * pass2에서 object code로 변환하는 과정은 혼자 해결할 수 없고 symbolTable과 instTable의 정보가 필요하므로 이를 링크시킨다.<br>
 * section 마다 인스턴스가 하나씩 할당된다.
 *
 */
public class TokenTable {
	public static final int MAX_OPERAND=3;

	/* bit 조작의 가독성을 위한 선언 */
	public static final int nFlag=32;
	public static final int iFlag=16;
	public static final int xFlag=8;
	public static final int bFlag=4;
	public static final int pFlag=2;
	public static final int eFlag=1;

	public static int tempLocation = -1;		//주소 없음 : -1

	/* Token을 다룰 때 필요한 테이블들을 링크시킨다. */
	SymbolTable symTab;
	LiteralTable literalTab;
	InstTable instTab;

	int length;	//최대 길이

	/** 각 line을 의미별로 분할하고 분석하는 공간. */
	ArrayList<Token> tokenList;

	/**
	 * 초기화하면서 literalTable, symTable과 instTable을 링크시킨다.
	 * @param symTab : 해당 section과 연결되어있는 symbol table
	 * @param literalTab : 해당 section과 연결되어있는 literal table
	 * @param instTab : instruction 명세가 정의된 instTable
	 */
	public TokenTable(SymbolTable symTab, LiteralTable literalTab, InstTable instTab) {
		//...
		this.symTab = symTab;
		this.instTab = instTab;
		this.literalTab = literalTab;
		this.tokenList = new ArrayList<>();
		this.length = 0;
	}


	/**
	 * 일반 문자열을 받아서 Token단위로 분리시켜 tokenList에 추가한다.
	 * @param line : 분리되지 않은 일반 문자열
	 */
	public void putToken(String line) {
		tokenList.add(new Token(line));
	}

	/**
	 * tokenList에서 index에 해당하는 Token을 리턴한다.
	 * @param index
	 * @return : index번호에 해당하는 코드를 분석한 Token 클래스
	 */
	public Token getToken(int index) {
		return tokenList.get(index);
	}

	/**
	 * Pass2 과정에서 사용한다.
	 * instruction table, symbol table literal table 등을 참조하여 objectcode를 생성하고, 이를 저장한다.
	 * @param index
	 */
	public void makeObjectCode(int index){
		Token tempToken = getToken(index);
		int tempOpcode = 0;
		int check = 0;
		int pc = 0;
		int num = 0;
		int locctr = 0;
		String tmp = null;
		if(tempToken.operator != null) {
			if((int)tempToken.nixbpe != 0 && tempToken.nixbpe != '4') {		//instList안에 있는 경우
				StringTokenizer n = new StringTokenizer(tempToken.operator, "+\n");
				check = tempToken.operator.indexOf("+");


				if(check != -1) {		//+가 있을 경우
					getToken(index).byteSize = 4;
					tempOpcode = instTab.instMap.get(n.nextToken()).opcode;

					if(tempOpcode < 16) {
						tempToken.objectCode = "0";
						tempOpcode >>= 2;
					tempOpcode <<= 6;
					tempOpcode |= (int)tempToken.nixbpe;
					tempToken.objectCode += Integer.toHexString(tempOpcode).toUpperCase();
					if(tempToken.operand[0] != null) {
						for(int c = 0; c < symTab.extrefList.size(); c++) {
							if(tempToken.operand[0].equals(symTab.extrefList.get(c))) {
								tempToken.objectCode += "00000";
							}
						}
					}
					}
					else {
						tempOpcode >>= 2;
						tempOpcode <<= 6;
						tempOpcode |= (int)tempToken.nixbpe;
						tempToken.objectCode = Integer.toHexString(tempOpcode).toUpperCase();
						if(tempToken.operand[0] != null) {
							for(int c = 0; c < symTab.extrefList.size(); c++) {
								if(tempToken.operand[0].equals(symTab.extrefList.get(c))) {
									tempToken.objectCode += "00000";
								}
							}
						}
					}
					getToken(index).objectCode = tempToken.objectCode;
				}
				else {	//+가 없을 경우
					tempOpcode = instTab.instMap.get(n.nextToken()).opcode;
					getToken(index).byteSize = 3;
					if(tempToken.operator.equals("RSUB")) {
						tempOpcode >>= 2;
							tempOpcode <<= 6;
							tempOpcode |= (int)tempToken.nixbpe;
							tempToken.objectCode = Integer.toHexString(tempOpcode).toUpperCase();
							tempToken.objectCode += "000";
							getToken(index).objectCode = tempToken.objectCode;
					}
					else if(tempOpcode < 16) {
						tempToken.objectCode = "0";
						tempOpcode >>= 2;
					tempOpcode <<= 6;
					tempOpcode |= (int)tempToken.nixbpe;
					tempToken.objectCode += Integer.toHexString(tempOpcode).toUpperCase();

					}
					else {
						tempOpcode >>= 2;
					tempOpcode <<= 6;
					tempOpcode |= (int)tempToken.nixbpe;
					tempToken.objectCode = Integer.toHexString(tempOpcode).toUpperCase();
					}

					if(tempToken.operand[0] != null) {
						if(tempToken.operand[1] == null) {
							if(tempToken.operand[0].charAt(0) == '#') {
								StringTokenizer z = new StringTokenizer(tempToken.operand[0], "#");
								tmp = z.nextToken();
								if(Integer.parseInt(tmp) < 16) {
									tempToken.objectCode += "00";
									tempToken.objectCode += tmp.toUpperCase();
								}
								else if(Integer.parseInt(tmp) <256 && Integer.parseInt(tmp) >= 16) {
									tempToken.objectCode += "0";
									tempToken.objectCode += tmp.toUpperCase();
								}
							}
							else if(tempToken.operand[0].charAt(0) == '@') {
								StringTokenizer z = new StringTokenizer(tempToken.operand[0], "@");
								tmp = z.nextToken();
								locctr = symTab.search(tmp);
								pc = tempToken.location +  instTab.instMap.get(tempToken.operator).format;
								num = locctr - pc;
								if(num >= 0) {
									if(Integer.toHexString(num).length() == 1) {
										tempToken.objectCode += ("00"+ Integer.toHexString(num).toUpperCase());
									}
									else if(Integer.toHexString(num).length() == 2) {
										tempToken.objectCode += ("0"+ Integer.toHexString(num).toUpperCase());
									}
									else if(Integer.toHexString(num).length() == 3){
										tempToken.objectCode += (Integer.toHexString(num).toUpperCase());
									}
									else if(Integer.toHexString(num).length() == 4) {
										tempToken.objectCode += Integer.toHexString(num).substring(Integer.toHexString(num).
												length()-3,Integer.toHexString(num).length()).toUpperCase();	
									}
								}
								else if(num < 0) {
									tempToken.objectCode += Integer.toHexString(num).substring(Integer.toHexString(num).
											length()-3,Integer.toHexString(num).length());
								}
							}
							else if(tempToken.operand[0].charAt(0) == '='){
								StringTokenizer z = new StringTokenizer(tempToken.operand[0], "'");
								z.nextToken();
								tmp = z.nextToken();	
								locctr = literalTab.search(tmp);
								pc = tempToken.location +  instTab.instMap.get(tempToken.operator).format;
								num = locctr - pc;
								if(num >= 0) {
									if(Integer.toHexString(num).length() == 1) {
										tempToken.objectCode += ("00"+ Integer.toHexString(num).toUpperCase());
									}
									else if(Integer.toHexString(num).length() == 2) {
										tempToken.objectCode += ("0"+ Integer.toHexString(num).toUpperCase());
									}
									else if(Integer.toHexString(num).length() == 3){
										tempToken.objectCode += (Integer.toHexString(num).toUpperCase());
									}
									else if(Integer.toHexString(num).length() == 4) {
										tempToken.objectCode += Integer.toHexString(num).substring(Integer.toHexString(num).
												length()-3,Integer.toHexString(num).length()).toUpperCase();	
									}
								}
								else if(num < 0) {
									tempToken.objectCode += Integer.toHexString(num).substring(Integer.toHexString(num).
											length()-3,Integer.toHexString(num).length()).toUpperCase();
								}
							}
							else {
								locctr = symTab.search(tempToken.operand[0]);
								pc = tempToken.location +  instTab.instMap.get(tempToken.operator).format;
								num = locctr - pc;
								if(num >= 0) {
									if(Integer.toHexString(num).length() == 1) {
										tempToken.objectCode += ("00"+ Integer.toHexString(num).toUpperCase());
									}
									else if(Integer.toHexString(num).length() == 2) {
										tempToken.objectCode += ("0"+ Integer.toHexString(num).toUpperCase());
									}
									else if(Integer.toHexString(num).length() == 3){
										tempToken.objectCode += (Integer.toHexString(num).toUpperCase());
									}
									else if(Integer.toHexString(num).length() == 4) {
										tempToken.objectCode += Integer.toHexString(num).substring(Integer.toHexString(num).
												length()-3,Integer.toHexString(num).length()).toUpperCase();	
									}
								}
								else if(num < 0) {
									tempToken.objectCode += Integer.toHexString(num).substring(Integer.toHexString(num).
											length()-3,Integer.toHexString(num).length()).toUpperCase();
								}
							}
							getToken(index).objectCode = tempToken.objectCode;
						}											
					}
				}
			}
			else {
				if(tempToken.nixbpe == '4') {
					if(tempToken.operand[0] != null) {
						getToken(index).byteSize = 2;
						tempOpcode = instTab.instMap.get(tempToken.operator).opcode;
						tempToken.objectCode = Integer.toHexString(tempOpcode).toUpperCase();
						if(tempToken.operand[0] != null) {
							if(tempToken.operand[0].equals("A")){
								tempToken.objectCode += "0";
							}
							else if(tempToken.operand[0].equals("X")) {
								tempToken.objectCode += "1";
							}
							else if(tempToken.operand[0].equals("S")) {
								tempToken.objectCode += "4";
							}
							else if(tempToken.operand[0].equals("T")) {
								tempToken.objectCode += "5";
							}


							if(tempToken.operand[1] == null) {
								tempToken.objectCode += "0";
							}
							else {
								if(tempToken.operand[1].equals("A")){
									tempToken.objectCode += "0";
								}
								else if(tempToken.operand[1].equals("X")) {
									tempToken.objectCode += "1";
								}
								else if(tempToken.operand[1].equals("S")) {
									tempToken.objectCode += "4";
								}
								else if(tempToken.operand[1].equals("T")) {
									tempToken.objectCode += "5";
								}
							}
						}
						getToken(index).objectCode = tempToken.objectCode;
					}
				}
				else if(tempToken.nixbpe == 0) {
					if(tempToken.operand[0] != null) {
						if(tempToken.operator.equals("BYTE")) {
							StringTokenizer v = new StringTokenizer(tempToken.operand[0], "'");
							v.nextToken();
							tmp = v.nextToken();
							getToken(index).byteSize = tmp.length()/2;
							tempToken.objectCode = tmp;

							getToken(index).objectCode = tempToken.objectCode;
						}
						else if(tempToken.operator.equals("WORD")) {
							StringTokenizer p = new StringTokenizer(tempToken.operand[0],"+-\n");
							while(p.hasMoreTokens()) {
								tmp = p.nextToken();
								for(int a = 0; a < symTab.extrefList.size(); a++) {
									if(symTab.extrefList.get(a).equals(tmp)) {
										tempToken.objectCode = "000000";
										getToken(index).byteSize = 3;
									}
								}
							}
							getToken(index).objectCode = tempToken.objectCode;
						}
					}
				}
			}
		}
	}

	/** 
	 * index번호에 해당하는 object code를 리턴한다.
	 * @param index
	 * @return : object code
	 */
	public String getObjectCode(int index) {
		return tokenList.get(index).objectCode;
	}

}

/**
 * 각 라인별로 저장된 코드를 단어 단위로 분할한 후  의미를 해석하는 데에 사용되는 변수와 연산을 정의한다. 
 * 의미 해석이 끝나면 pass2에서 object code로 변형되었을 때의 바이트 코드 역시 저장한다.
 */
class Token{
	//의미 분석 단계에서 사용되는 변수들
	int location;
	String label;
	String operator;
	String[] operand;
	String comment;
	char nixbpe;

	// object code 생성 단계에서 사용되는 변수들 
	String objectCode;
	int byteSize;

	/**
	 * 클래스를 초기화 하면서 바로 line의 의미 분석을 수행한다. 
	 * @param line 문장단위로 저장된 프로그램 코드
	 */
	public Token(String line) {
		//initialize 추가
		parsing(line);
	}

	/**
	 * line의 실질적인 분석을 수행하는 함수. Token의 각 변수에 분석한 결과를 저장한다.
	 * @param line 문장단위로 저장된 프로그램 코드.
	 */
	public void parsing(String line) {
		int tempLocation = TokenTable.tempLocation;		//주소 없음 : -1
		int count;
		this.nixbpe = 0;
		this.label = null; this.operator=null; this.comment = null;
		this.operand = new String[3];
		String tempOperand = null, tempOperand1 = null, tempOperand2 = null, tempOperand3 = null; 
		String tmp = null;

		String split[] = line.split("\t");
		if(split.length >= 1) {		//토큰화 
			if(split[0].isEmpty()) 
				this.label = null;
			else if(split[0].equals(".")) {
				this.comment = line;
				this.location = -1;
				return;
			}
			else
				this.label = split[0];

			if(split.length >= 2) {
				if(split[1].equals(" "))
					this.operator = null;
				else
					this.operator = split[1];

				if(split.length >= 3) {
					tempOperand = split[2];

					StringTokenizer x = new StringTokenizer(tempOperand, ",\n");
					count = 0;
					while(x.hasMoreTokens()) {
						if(count > 3) {
							break;
						}
						if(count == 0) {
							tempOperand1 = x.nextToken();
							operand[0] = new String(tempOperand1);
						}
						else if(count == 1) {
							tempOperand2 = x.nextToken();
							operand[1] = tempOperand2;	
						}
						else if(count == 2) {
							tempOperand3 = x.nextToken();
							operand[2] = tempOperand3;
						}
						count++;
					}
					if(split.length == 4) {
						comment = split[3];
						if(split[3].equals("  "))
							this.comment = null;
						else
							this.comment = split[3];
					}
				}
			}
		}
		if(operator != null) {		//주소 계산
			if(operator.equals("START") || operator.equals("CSECT")) {
				tempLocation = 0;
				location = tempLocation;
			}
			else if(operator.charAt(0) == '+') {
				if(operand[1] != null && operand[1].equals("X")) {
					setFlag(TokenTable.nFlag|TokenTable.iFlag|TokenTable.xFlag|TokenTable.eFlag, 1);
					setFlag(TokenTable.bFlag|TokenTable.pFlag, 0);
				}
				else if(operand[1] == null) {
					setFlag(TokenTable.nFlag|TokenTable.iFlag|TokenTable.eFlag, 1);
					setFlag(TokenTable.xFlag|TokenTable.bFlag|TokenTable.pFlag, 0);
				}
				location = tempLocation;
				tempLocation += 4;
			}
			else if(operator.equals("RESB")) {
				location = tempLocation;
				tempLocation += Integer.parseInt(operand[0]);
			}
			else if(operator.equals("RESW")) {
				location = tempLocation;
				tempLocation += Integer.parseInt(operand[0]) * 3;
			}
			else if(operator.equals("EXTDEF") || operator.equals("EXTREF")) {
				location = -1;
			}
			else if(operator.equals("EQU")) {
				if(operand[0] != null && operand[0].equals("*")) {
					location = tempLocation;
					//					tempLocation += 3;
				}
				else if(label != null && label.equals("MAXLEN")) {
					location = tempLocation;
				}
			}
			else if(operator.equals("BYTE")) {
				location = tempLocation;
				if(operand[0] != null && operand[0].substring(0,2).equals("X'")){ 
					StringTokenizer z = new StringTokenizer(operand[0],"'");
					z.nextToken();
					tmp = z.nextToken();
					tempLocation += (tmp.length() / 2);
				}
			}
			else if(operator.equals("WORD")) {			//WORD일 경우 주소는 assembler에서 계산한다.
				location = tempLocation;	
			}
			else if(operator.equals("LTORG") || operator.equals("END")) {			//LTORG일 경우 주소는 assembler에서 계산한다.
				location = -1;
			}
			else {
				location = tempLocation;
				if(operand[0] != null) {
					if(operand[0].charAt(0) == '#') {
						setFlag(TokenTable.iFlag, 1);
						setFlag(TokenTable.nFlag|TokenTable.xFlag|TokenTable.bFlag|TokenTable.pFlag|TokenTable.eFlag, 0);

					}
					else if(operand[0].charAt(0) == '@') {
						setFlag(TokenTable.nFlag|TokenTable.pFlag, 1);
						setFlag(TokenTable.iFlag|TokenTable.xFlag|TokenTable.bFlag|TokenTable.eFlag, 0);
					}
					else {
						setFlag(TokenTable.nFlag|TokenTable.iFlag|TokenTable.pFlag, 1);
						setFlag(TokenTable.xFlag|TokenTable.bFlag|TokenTable.eFlag, 0);
					}
				}
				else {
					if(operator.equals("RSUB")) {
						setFlag(TokenTable.nFlag|TokenTable.iFlag, 1);
						setFlag(TokenTable.xFlag|TokenTable.bFlag|TokenTable.pFlag|TokenTable.eFlag, 0);
					}
				}
			}
			TokenTable.tempLocation = tempLocation;
		}
		else {		//operator가 없을 때
			location = -1;
		}
	}

	/** 
	 * n,i,x,b,p,e flag를 설정한다. 
	 * @param flag : 원하는 비트 위치
	 * @param value : 집어넣고자 하는 값. 1또는 0으로 선언한다.
	 */
	public void setFlag(int flag, int value) {
		nixbpe |= flag * value;
	}

	/**
	 * 원하는 flag들의 값을 얻어올 수 있다. flag의 조합을 통해 동시에 여러개의 플래그를 얻는 것 역시 가능하다 
	 * @param flags : 값을 확인하고자 하는 비트 위치
	 * @return : 비트위치에 들어가 있는 값. 플래그별로 각각 32, 16, 8, 4, 2, 1의 값을 리턴할 것임.
	 */
	public int getFlag(int flags) {
		return nixbpe & flags;
	}
}
