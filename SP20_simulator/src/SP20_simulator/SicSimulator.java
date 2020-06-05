package SP20_simulator;

import java.io.File;
import java.util.ArrayList;

public class SicSimulator {		//시뮬레이터로서의 작업
	ResourceManager rMgr;
	InstLuncher instLuncher;
	int pc;
	int pcReg;
	char nixbpe;
	int type;
	static String inst;
	char[] step;
	static int targetAddr;
	ArrayList<String> logList = new ArrayList<String>();
	ArrayList<String> instList = new ArrayList<String>();

	public SicSimulator(ResourceManager resourceManager) {		//생성자
		pc = 0;
		pcReg = 0;
		nixbpe = 0;
		type = 0;
		inst = new String();
		targetAddr = 0;
		this.rMgr = resourceManager;
		this.instLuncher = new InstLuncher(resourceManager);
	}

	public void load(File program) {	//load
	}

	public void findInst(char[] bits) {			//명령어와 형식 nixbpe를 구하는 함수
		char check = 0;
		nixbpe = 0; inst = " "; type = 0;
		if(bits[0] == '0') {
			if(bits[1] == 'F') {
				inst = "STA";
				nixbpe |= 32;
				nixbpe |= 16;
				type = 3;
			}
			else if(bits[1] == '1') {
				inst = "LDA";
				nixbpe |= 16;
				type = 3;
			}
			else if(bits[1] == '3') {
				inst = "LDA";
				nixbpe |= 32;
				nixbpe |= 16;
				type = 3;
			}

		}
		else if(bits[0] == '1') {
			nixbpe |= 32;
			nixbpe |= 16;
			if(bits[1] == '3') {
				inst = "STX";
				type = 3;
			}
			else if(bits[1] == '7') {
				inst = "STL";
				type = 3;
			}
		}
		else if(bits[0] == '2') {
			nixbpe |= 32; nixbpe |= 16;
			if(bits[1] == '9') {
				inst = "COMP";
				nixbpe &= 0; nixbpe |= 16;
				type = 3;
			}
		}
		else if(bits[0] == '3') {
			nixbpe |= 32; nixbpe |= 16;
			if(bits[1] == '3') {
				inst = "JEQ";
				type = 3;
			}
			else if(bits[1] == 'B') {
				inst = "JLT";
				type = 3;
			}
			else if(bits[1] == 'E') {
				inst = "J";
				nixbpe &= 0; nixbpe |= 32;
				type = 3;
			}
			else if(bits[1] == 'F') {
				inst ="J";
				type = 3;
			}
		}
		else if(bits[0] == '4') {
			nixbpe |= 32; nixbpe |= 16;
			if(bits[1] == 'B') {
				inst = "JSUB";
				type = 3;
			}
			else if(bits[1] == 'F') {
				inst = "RSUB";
				type = 3;
			}
		}
		else if(bits[0] == '5') {
			nixbpe |= 32; nixbpe |= 16;
			if(bits[1] == '3') {
				inst = "LDCH";
				type = 4;
			}
			else if(bits[1] == '7') {
				inst = "STCH";
				type = 4;
			}

		}
		else if(bits[0] == '7') {
			nixbpe |= 32; nixbpe |= 16;
			if(bits[1] == '7') {
				inst = "LDT";
				type = 3;
			}

		}
		else if(bits[0] == 'A') {
			if(bits[1] == '0') {
				inst = "COMPR";
				type = 2;
			}
		}
		else if(bits[0] == 'B') {
			if(bits[1] == '4') {
				inst = "CLEAR";
				type = 2;
			}
			else if(bits[1] == '8') {
				inst = "TIXR";
				type = 2;
			}
		}
		else if(bits[0] == 'D') {
			nixbpe |= 32; nixbpe |= 16;
			if(bits[1] == 'B') {
				inst = "RD";
				type = 3;
			}
			else if(bits[1] == 'F') {
				inst = "WD";
				type = 3;
			}
		}
		else if(bits[0] == 'E') {
			nixbpe |= 32; nixbpe |= 16;
			if(bits[1] == '3') {
				inst = "TD";
				type = 3;
			}
		}
		if(type == 2) {
			//2형식은 그냥 씀
		}
		else if(type == 3) {
			if(bits[2] == '2') {	//p = 1
				nixbpe |= 2;
			}
			else if(bits[2] == '1') {	//e = 1
				nixbpe |= 1;
				type = 4;
			}
			else if(bits[2] == '9') {	//x레지스터 
				nixbpe |= 9;
			}
			else if(bits[2] == '0') {
			}
		}
	}

	public void doingInst(String inst, char[] step, int type, int pcctr) {		//명령어를 실행하는 함수
		if(inst.equals("LDA")) {
			instLuncher.LDA(step, pcctr,type, nixbpe);
		}
		else if(inst.equals("STA")) {
			instLuncher.STA(step, pcctr, type);
		}
		else if(inst.equals("STX")) {
			instLuncher.STX(step, pcctr, type);
		}
		else if(inst.equals("STL")) {
			instLuncher.STL(step, pcctr, type);
		}
		else if(inst.equals("COMP")) {
			instLuncher.COMP(step, nixbpe);
		}
		else if(inst.equals("JEQ")) {
			int temp = 0;
			temp = instLuncher.JEQ(step,pcctr,type);
			pc = temp;
			pcReg = temp;
		}
		else if(inst.equals("J")) {			
			int temp = 0;
			temp = instLuncher.J(step,pcctr,type, nixbpe);
			pc = temp;
			pcReg = temp;
		}
		else if(inst.equals("JLT")) {
			int temp = 0;
			temp = instLuncher.JLT(step,pcctr,type);
			pc = temp;
			pcReg = temp;
		}
		else if(inst.equals("JSUB")) {
			int temp = 0;
			temp = instLuncher.JSUB(step,pcctr,type);
			pc = temp;
			pcReg =temp;
		}
		else if(inst.equals("RSUB")) {
			int temp = 0;
			temp = instLuncher.RSUB(type);
			pc = temp;
		}
		else if(inst.equals("LDCH")) {
			instLuncher.LDCH(step,pcctr,type);
		}
		else if(inst.equals("STCH")) {
			instLuncher.STCH(step,pcctr,type, nixbpe);
		}
		else if(inst.equals("LDT")) {		
			instLuncher.LDT(step,pcctr, type);	
		}
		else if(inst.equals("CLEAR")) {
			instLuncher.CLEAR(step);
		}
		else if(inst.equals("COMPR")) {		
			instLuncher.COMPR(step);
		}
		else if(inst.equals("TIXR")) {
			instLuncher.TIXR(step);
		}
		else if(inst.equals("WD")) {
			instLuncher.WD(step, pcctr, type);
		}
		else if(inst.equals("TD")) {
			instLuncher.TD(step, pcctr, type);	
		}
		else if(inst.equals("RD")) {
			instLuncher.RD(step, pcctr, type);
		}
	}

	public void oneStep() {		//1개의 instruction이 수행
		char[] find = rMgr.getMemory(pc, 3);	

		findInst(find);

		if(type == 2) {
			targetAddr = 0;
			step = rMgr.getMemory(pc,4);
			doingInst(inst,step,type,pc);
			pc += 2*2;
		}
		else if(type == 3) {
			step = rMgr.getMemory(pc,6);
			doingInst(inst, step,type,pc);
			pc += 3*2;
			if(inst == "J") {
				pc -= 3*2;
			}	
		}
		else if(type == 4) {
			step = rMgr.getMemory(pc,8);
			doingInst(inst,step,type,pc);
			if(inst != "JSUB" ) {
				pc += 4*2;
			}	
		}
		else {
		}
		addLog(inst);
	}	
	public void allStep() {		// 남은 모든 instruction이 수행됨
		while(true) {
			if(rMgr.eFlag == 1)
				break;
			oneStep();
		}
	}
	public void addLog(String log) {		//관련된 기록을 남김
		logList.add(log);
		instList.add(String.valueOf(step));
	}	
}
