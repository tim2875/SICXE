package SP20_simulator;

public class InstLuncher { // instruction에 따라 동작을 수행하는 메소드를 정의하는 클래스
    ResourceManager rMgr;
    char[] addr3;
    char[] addr4;
    char[] addr2;
    
    public InstLuncher(ResourceManager resourceManager) {
        this.rMgr = resourceManager;
        addr3 = new char[3];
        addr4 = new char[5];
        addr2 = new char[2];
    }
    // instruction 별로 동작을 수행하는 메소드

    public String decToHex(int dec) {	//10진수를 16진수 문자열로 변환하는 함수
		String tempHex = new String();
		String temp = new String();
		char tempCh;
		int tempInt = 0;
		tempHex = Integer.toHexString(dec).toUpperCase();
		if(tempHex.length() != 6) {
			for(int i = tempHex.length(); i < 6;i++) {
				if(i == tempHex.length())
					temp = "0";
				else {
					temp += "0";
				}
			}
			temp += tempHex;
		}
		else {
			tempInt = Integer.parseInt(tempHex.substring(0,2),16);
			tempCh = (char)tempInt;
			temp = String.valueOf(tempCh);
			tempInt = Integer.parseInt(tempHex.substring(2,4),16);
			tempCh = (char)tempInt;
			temp += String.valueOf(tempCh);
			tempInt = Integer.parseInt(tempHex.substring(4,6),16);
			tempCh = (char)tempInt;
			temp += String.valueOf(tempCh);
			
			for(int i = temp.length(); i <6; i++) {
				temp += "0";
			}
		}
		return temp;
	}
    
    public void LDA(char[] objcode,int pc, int type, char nixbpe) {	
    	int num = 0;
    	String temp = new String();
    	String str = new String();
    	String str2 = new String();
    	char[] rst = new char[1];

    	for(int i = 3; i < 6; i++) {
    		addr3[i-3] = objcode[i];
    	}
    	temp = String.valueOf(addr3);
    	str = Integer.toHexString(pc/2+type);
    	if(addr3[0] == '0')
    		num = (pc/2+type + rMgr.charToInt(addr3))*2;
    	else if(addr3[0] == 'F') {
    		str2 = str.substring(1,4);
    		num = (Integer.parseInt(str2,16)+Integer.parseInt(temp,16))*2;
    	}
    	if((nixbpe & 48) == 48) {

    		rst = rMgr.getMemory(num, 6);
    		rMgr.setRegister(0, Integer.parseInt(String.valueOf(rst),16));

    		SicSimulator.targetAddr = num/2;
    	}
    	else if((nixbpe & 48) == 16) {
    		rMgr.setRegister(0, Integer.parseInt(String.valueOf(addr3),16));
    	}
    }
    public void STA(char[] objcode, int pc, int type) {	
    	int num = 0;
    	String temp = new String();
    	String str = new String();
    	String str2 = new String();
    	char[] rst = null;

    	for(int i = 3; i < 6; i++) {
    		addr3[i-3] = objcode[i];
    	}
    	temp = String.valueOf(addr3);
    	str = Integer.toHexString(pc/2+type);
    	if(addr3[0] == '0')
    		num = (pc/2+type + rMgr.charToInt(addr3))*2;
    	else if(addr3[0] == 'F') {
    		str2 = str.substring(1,4);
    		num = (Integer.parseInt(str2,16)+Integer.parseInt(temp,16))*2;
    	}
    	SicSimulator.targetAddr = num/2;
    	temp = decToHex(rMgr.register[0]);
    	rMgr.setMemory(num,temp.toCharArray(),6);
    }
    public void STX(char[] objcode, int pc, int type) {		
    	String temp = new String();
    	int num;
    	int tempInt;
    	if(type == 3){
    		for(int i = 3; i < 6; i++) {
    			addr3[i-3] = objcode[i];
    		}
    		num = pc+type*2 + rMgr.charToInt(addr3)*2;
    		SicSimulator.targetAddr = num/2;
    		temp = decToHex(rMgr.register[1]);

    		rMgr.setMemory(pc,temp.toCharArray(),6);
    	}
    	else if(type == 4) {
    		for(int i = 3; i < 8; i++) {
    			addr4[i-3] = objcode[i];
    		}
    		temp = String.valueOf(addr4);
    		num = Integer.parseInt(temp, 16)*2;
    		tempInt = rMgr.getRegister(1);
    		
    		rMgr.setMemory(num,decToHex(tempInt).toCharArray(),6);
    		SicSimulator.targetAddr = num/2;
    	}
    	
    }
    public void STL(char[] objcode, int pc, int type) {		
    	int num = 0;
    	String temp = new String();
    	String str = new String();
    	String str2 = new String();
    	
    	for(int i = 3; i < 6; i++) {
			addr3[i-3] = objcode[i];
		}
		temp = String.valueOf(addr3);
		str = Integer.toHexString(pc/2+type);
		if(addr3[0] == '0')
			num = (pc/2+type + rMgr.charToInt(addr3))*2;
		else if(addr3[0] == 'F') {
			str2 = str.substring(1,4);
			num = (Integer.parseInt(str2,16)+Integer.parseInt(temp,16))*2;
		}
    	SicSimulator.targetAddr = num/2;
    	temp = decToHex(rMgr.register[2]);
    	rMgr.setMemory(num,temp.toCharArray(),6);
    }

    
    public void COMP(char[] objcode, char nixbpe) {	
    	int aRegNum;
    	if((nixbpe & 48) == 16) {
    		for(int i = 3; i < 6; i++) {
    			addr3[i-3] = objcode[i];
    		}
    		aRegNum = rMgr.getRegister(0);
    		if(aRegNum > rMgr.charToInt(addr3)) {	//클 때
    			rMgr.setRegister(9, 2);
    		}
    		else if(aRegNum <  rMgr.charToInt(addr3)) {	//작을 때	
    			rMgr.setRegister(9, 1);
    		}
    		else if(aRegNum == rMgr.charToInt(addr3)) { //같을 때
    			rMgr.setRegister(9, 0);
    		}
    	}
    }
    public int JEQ(char[] objcode, int pc, int type) {	
    	int num = 0;
    	String temp = new String();
    	String str = new String();
    	String str2 = new String();
    	if(rMgr.getRegister(9) == 0){
    		for(int i = 3; i < 6; i++) {
    			addr3[i-3] = objcode[i];
    		}
    		temp = String.valueOf(addr3);
    		str = Integer.toHexString(pc/2+type);

    		if(addr3[0] == '0')
    			num = pc+type*2 + rMgr.charToInt(addr3)*2;
    		else if(addr3[0] == 'F') {
    			str2 = str.substring(1,4);
    			num = (Integer.parseInt(str2,16)+Integer.parseInt(temp,16))*2;
    		}
    			num = num -type*2;
    		SicSimulator.targetAddr = num/2;
    		rMgr.setRegister(8, num+2*type);
    		return num;
    	}
    	else {
    		SicSimulator.targetAddr = pc/2;
    		return pc;
    	}
    }
    
    public int J(char[] objcode, int pc, int type, int nixbpe) {	
    	int num = 0;
    	String temp = new String();
    	String tempStr = new String();
    	String str = new String();
    	String str2 = new String();
    	char[] test = new char[6];
    	for(int i = 3; i < 6; i++) {
    		addr3[i-3] = objcode[i];
    	}
		temp = String.valueOf(addr3);
		str = decToHex(pc/2+type);
		if(addr3[0] == '0')
			num = (pc/2+type + rMgr.charToInt(addr3))*2;
		else if(addr3[0] == 'F') {
			str2 = str.substring(4);
			tempStr = Integer.toHexString((Integer.parseInt(str2,16)+Integer.parseInt(temp,16)));
			num = Integer.parseInt(tempStr.substring(1,4));
		}
		if((nixbpe & 48) == 32 && (nixbpe & 48) != 48) {			
			test = rMgr.getMemory(num, 6);
			num = Integer.parseInt(String.valueOf(test), 16);
			rMgr.eFlag = 1;
		}
		rMgr.setRegister(8, num*2);
		SicSimulator.targetAddr = num;
		return num*2;
    }
    public int JLT(char[] objcode, int pc, int type) {	
    	int num = 0;
    	String temp = new String();
    	String str = new String();
    	String str2 = new String();
    	if(rMgr.getRegister(9) == 1){
    		for(int i = 3; i < 6; i++) {
        		addr3[i-3] = objcode[i];
        	}
    		temp = String.valueOf(addr3);
    		str = Integer.toHexString(pc/2+type);
    		
    		if(addr3[0] == '0')
    			num = pc+type*2 + rMgr.charToInt(addr3)*2;
    		else if(addr3[0] == 'F') {
    			str2 = str.substring(1,4);
    			num = (Integer.parseInt(str2,16)+Integer.parseInt(temp,16))*2;
    		}
    		num = num-type*2;	
    		SicSimulator.targetAddr = num/2;
    		rMgr.setRegister(8, num);
    		return num;
    	}
    	else {
    		return pc;
    	}
    	
    }
    public int JSUB(char[] objcode, int pc, int type) {
    	int pcctr = 0;
    	String tempstr = new String();
    	for(int i = 3; i < 8; i++) {
    		addr4[i-3] = objcode[i];
    	}
    	tempstr = String.valueOf(addr4);
    	pcctr = Integer.parseInt(tempstr,16) * 2;
    	rMgr.setRegister(2, pc/2 + type);					//L레지스터에 

    	SicSimulator.targetAddr = rMgr.charToInt(addr4);	//target address
    	rMgr.setRegister(8, pcctr);							//PC레지스터
    	
    	return pcctr;
    }
    public int RSUB(int type) {	//4C
    	int num = 0;
    	num = rMgr.getRegister(2);
    	rMgr.usingDev = "";
    	SicSimulator.targetAddr = (num)/2;					//target address
    	return num *2 - type*2;
    }
    public void LDCH(char[] objcode, int pc, int type) {	
     	int num = 0;
    	String temp = new String();
    	String str = new String();
    	String str2 = new String();
    	char[] rst = new char[1];
    	
    	if(type == 4) {
    		for(int i = 3; i < 8; i++) {
    			addr4[i-3] = objcode[i];
    		}
    		temp = String.valueOf(addr4);
    		num = Integer.parseInt(temp, 16)*2+rMgr.getRegister(1);
    		rst = rMgr.getMemory(num, 1);
    		rMgr.setRegister(0, (int)rst[0]);				//A레지스터
    	}
    	SicSimulator.targetAddr = (num)/2;					//target address
    }
    public void STCH(char[] objcode,int pc, int type, char nixbpe) {	
    	int num = 0;
    	String temp = new String();
    	String str = new String();
    	String str2 = new String();
    	char[] rst = new char[1];
    	
    	if(type == 3) {
    		for(int i = 3; i < 6; i++) {
    			addr3[i-3] = objcode[i];
    		}
    		temp = String.valueOf(addr3);
    		str = Integer.toHexString(pc/2+type);

    		if(addr3[0] == '0')
    			num = pc+type*2 + rMgr.charToInt(addr3)*2;
    		else if(addr3[0] == 'F') {
    			str2 = str.substring(1,4);
    			num = (Integer.parseInt(str2,16)+Integer.parseInt(temp,16))*2;
    		}
    	}
    	else if(type == 4) {
    		for(int i = 3; i < 8; i++) {
    			addr4[i-3] = objcode[i];
    		}
    		temp = String.valueOf(addr4);
    		num = Integer.parseInt(temp, 16)*2+rMgr.getRegister(1);
    		rst[0] = (char)rMgr.getRegister(0);
    		rMgr.setMemory(num, rst, 1);
    	}
    		
    		SicSimulator.targetAddr = (num)/2;				//target address
    	
    }  
    public void LDT(char[] objcode,int pc, int type) {		
    	int num = 0;
    	String temp = new String();
    	String str = new String();
    	String str2 = new String();
    	char[] rst = new char[1];
    	
    	if(type == 3) {
    		for(int i = 3; i < 6; i++) {
    			addr3[i-3] = objcode[i];
    		}
    		temp = String.valueOf(addr3);
    		str = Integer.toHexString(pc/2+type);
    		if(addr3[0] == '0')
    			num = (pc/2+type + rMgr.charToInt(addr3))*2;
    		else if(addr3[0] == 'F') {
    			str2 = str.substring(1,4);
    			num = (Integer.parseInt(str2,16)+Integer.parseInt(temp,16))*2;
    		}
    			rst = rMgr.getMemory(num, 6);
    			rMgr.setRegister(5, Integer.parseInt(String.valueOf(rst),16));

    		SicSimulator.targetAddr = num/2;				//target address
    	}
    	else if(type == 4) {
    		for(int i = 3; i < 8; i++) {
    			addr4[i-3] = objcode[i];
    		}
    		temp = String.valueOf(addr4);
    		str = Integer.toHexString(pc/2+type);

    		if(addr4[0] == '0')
    			num = (pc/2+type + rMgr.charToInt(addr4))*2;
    		else if(addr4[0] == 'F') {
    			str2 = str.substring(1,4);
    			num = (Integer.parseInt(str2,16)+Integer.parseInt(temp,16))*2;
    		}
    		rst = rMgr.getMemory(Integer.parseInt(temp,16)*2, 6);
    		rMgr.setRegister(5, Integer.parseInt(String.valueOf(rst),16));
    		
    		SicSimulator.targetAddr = num/2;				//target address
    	}
    }

    public void CLEAR(char[] objcode) {
    	int regNum;
    	regNum = Character.getNumericValue(objcode[2]);
    	rMgr.setRegister(regNum, 0);
    }
    
    public void TD(char[] objcode, int pc, int type) {		

    	int num = 0;
    	String temp = new String();
    	String str = new String();
    	String str2 = new String();
    	char[] rst = null;
    		for(int i = 3; i < 6; i++) {
    			addr3[i-3] = objcode[i];
    		}
    		temp = String.valueOf(addr3);
    		str = Integer.toHexString(pc/2+type);

    		if(addr3[0] == '0')
    			num = pc+type*2 + rMgr.charToInt(addr3)*2;
    		else if(addr3[0] == 'F') {
    			str2 = str.substring(1,4);
    			num = (Integer.parseInt(str2,16)+Integer.parseInt(temp,16))*2;
    		}
    		
    		rst = rMgr.getMemory(num, 2);
        	rMgr.testDevice(String.valueOf(rst));
    		SicSimulator.targetAddr = num/2;				//target address

    }	
    public void RD(char[] objcode, int pc, int type) {		
    	int num = 0;
    	int test = 0;
    	String temp = new String();
    	String str = new String();
    	String str2 = new String();
    	char[] rst = null;
    	char[] fromFile = null;

    		for(int i = 3; i < 6; i++) {
    			addr3[i-3] = objcode[i];
    		}
    		temp = String.valueOf(addr3);
    		str = Integer.toHexString(pc/2+type);

    		if(addr3[0] == '0')
    			num = pc+type*2 + rMgr.charToInt(addr3)*2;
    		else if(addr3[0] == 'F') {
    			str2 = str.substring(1,4);
    			num = (Integer.parseInt(str2,16)+Integer.parseInt(temp,16))*2;
    		}
    		
    		rst = rMgr.getMemory(num, 2);
        	fromFile = rMgr.readDevice(String.valueOf(rst), 1);
        	test = (int)fromFile[0];
        	rMgr.setRegister(0, test);				//A레지스터
        	
    		SicSimulator.targetAddr = num/2;		//target address
    }
    
    public void WD(char[] objcode, int pc, int type) {		//DC
    	int num = 0;
    	int test = 0;
    	String temp = new String();
    	String str = new String();
    	String str2 = new String();
    	char[] rst = null;
    	char[] toFile = new char[1];

    		for(int i = 3; i < 6; i++) {
    			addr3[i-3] = objcode[i];
    		}
    		temp = String.valueOf(addr3);
    		str = Integer.toHexString(pc/2+type);

    		if(addr3[0] == '0')
    			num = pc+type*2 + rMgr.charToInt(addr3)*2;
    		else if(addr3[0] == 'F') {
    			str2 = str.substring(1,4);
    			num = (Integer.parseInt(str2,16)+Integer.parseInt(temp,16))*2;
    		}

    		rst = rMgr.getMemory(num, 2);
    		toFile[0] = (char)rMgr.getRegister(0);
    		
        	rMgr.writeDevice(String.valueOf(rst), toFile , 1);
    		SicSimulator.targetAddr = num/2;			//target address
    }
    public void COMPR(char[] objcode) {
    	int regNum1;
       	int regNum2;
       	int rst1;
       	int rst2;
       	regNum1 = Character.getNumericValue(objcode[2]);
    	regNum2 = Character.getNumericValue(objcode[3]);
       	
    	rst1 = rMgr.getRegister(regNum1);
    	rst2 = rMgr.getRegister(regNum2);
    	if(rst1 > rst2) {			//> : 2
    		rMgr.setRegister(9, 2);
    	}
    	else if(rst1 == rst2) {		//= : 0
    		rMgr.setRegister(9, 0);
    	}
    	else if(rst1 < rst2) {		//< : 1
    		rMgr.setRegister(9, 1);
    	}
    }

    public void TIXR(char[] objcode) {	
    	int regZ = 0;
    	int regX = 0;
    	regZ = rMgr.getRegister(Character.getNumericValue(objcode[2]));
    	
    	regX = rMgr.getRegister(1);
    	regX += 1;
    	rMgr.setRegister(1, regX);
    	
    	if(regX > regZ) {
    		rMgr.setRegister(9, 2);
    	}
    	else if(regX < regZ) {
    		rMgr.setRegister(9,1);
    	}
    	else if(regX == regZ) {
    		rMgr.setRegister(9,0);
    	}
    }
}