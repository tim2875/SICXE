package SP20_simulator;

import java.awt.*;
import java.awt.event.*;
import java.io.File;

import javax.swing.*;
import javax.swing.border.LineBorder;
import javax.swing.border.TitledBorder;

/**
 * VisualSimulator는 사용자와의 상호작용을 담당한다.<br>
 * 즉, 버튼 클릭등의 이벤트를 전달하고 그에 따른 결과값을 화면에 업데이트 하는 역할을 수행한다.<br>
 * 실제적인 작업은 SicSimulator에서 수행하도록 구현한다.
 */
public class VisualSimulator extends JFrame{
	ResourceManager resourceManager = new ResourceManager();
	SicLoader sicLoader = new SicLoader(resourceManager);
	SicSimulator sicSimulator = new SicSimulator(resourceManager);

	Container mainContainer;
	JPanel topPanel;
	JPanel centerPanel;
	JPanel bottomPanel;
	JPanel leftPanel;
	JPanel rightPanel;
	JPanel header; JPanel headerLeft; JPanel headerRight;
	JPanel register; JPanel registerLeft; JPanel registerRight; JPanel registerCenter;
	JPanel registerXE; JPanel registerXELeft; JPanel registerXERight; JPanel registerXECenter;
	JPanel endPanel;
	JPanel addrPanel;
	JPanel instPanel; JPanel instPanelLeft; JPanel instPanelRight;
	JPanel instPanelBtn; JPanel instPanelDev;
	
	JButton btnOpen;
	JButton btnOneStep;
	JButton btnAllStep;
	JButton btnExit;
	
	JTextField nameOfFile;
	JTextField hProName;
	JTextField hStartAddr;
	JTextField hLength;
	JTextField eFirAddr;
	JTextField memStartAddr;
	JTextField targetAddr;
	
	JTextField regADec;
	JTextField regAHex;
	JTextField regXDec;
	JTextField regXHex;
	JTextField regLDec;
	JTextField regLHex;
	JTextField regPCDec;
	JTextField regPCHex;
	JTextField regSW;
	JTextField regBDec;
	JTextField regBHex;
	JTextField regSDec;
	JTextField regSHex;
	JTextField regTDec;
	JTextField regTHex;
	JTextField regF;
	JTextField usingDev;
	
	JList instructions;
	JList log;
	
	DefaultListModel model1 ;
	DefaultListModel model2 ;
	
	JMenuBar mb;
	JMenu File;
	JMenu About;
	
	int count;
	String fileName;
	
	VisualSimulator(String title){			//생성자
		super(title);
		createMenu();
		count = 0;
		fileName=new String();
		
		model1 = new DefaultListModel<>();
		model2 = new DefaultListModel<>();
		
		this.setSize(550,700);
		this.setLocation(50,30);
		this.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
		
		mainContainer=this.getContentPane();
		mainContainer.setLayout(new BorderLayout(0,0));
		mainContainer.setBackground(Color.lightGray);
		this.getRootPane().setBorder(BorderFactory.createMatteBorder(1,1,1,1, Color.WHITE));

		mkGrid();
	}
	
	void createMenu() {		//메뉴 생성 
		mb = new JMenuBar();
		File = new JMenu("File");
		About = new JMenu("About");
		JMenuItem open = new JMenuItem("open");
		
		MenuActionListener menuClick = new MenuActionListener();
		open.addActionListener((ActionListener) menuClick);
		
		mb.add(File);
		File.add(open);
		mb.add(About);
		setJMenuBar(mb);
	}
	
	public void mkGrid() {							//UI 제작함수
		JLabel Filename = new JLabel("FileName : ");
		JLabel labelLog = new JLabel("Log(명령어 수행 관련):");
		JLabel labelProName = new JLabel("Program Name: ");
		JLabel labelObjAddr = new JLabel("Start Address of ");
		JLabel labelObjAddr2 = new JLabel("   Object Program: ");
		JLabel labelHLength = new JLabel("Length of Program:");
		JLabel labelDec = new JLabel("Dec ");
		JLabel labelHex = new JLabel("Hex ");
		JLabel labelDec2 = new JLabel("Dec ");
		JLabel labelHex2 = new JLabel("Hex ");
		
		JLabel labelA = new JLabel("A(#0)"); JLabel labelX = new JLabel("X(#1)");
		JLabel labelL = new JLabel("L(#2)"); JLabel labelPC = new JLabel("PC(#8)");
		JLabel labelSW = new JLabel("SW(#9)"); JLabel labelB = new JLabel("B(#3) ");
		JLabel labelS = new JLabel("S(#4) "); JLabel labelT = new JLabel("T(#5) ");
		JLabel labelF = new JLabel("F(#6) ");
		
		JLabel labelFirAddr = new JLabel("Address of First Instruction");
		JLabel labelFirAddr2 = new JLabel(" in Object Program:");
		JLabel labelStartAddr = new JLabel("Start Address in Memory");
		JLabel labelTargetAddr = new JLabel("Target Address : ");
		JLabel labelInst = new JLabel("Instruction : ");
		JLabel labelDev = new JLabel("사용중인 장치");
		
		JLabel labelEmpty = new JLabel("  ");
		JLabel labelEmpty2 = new JLabel("  ");
		JLabel labelEmpty3 = new JLabel("  ");
		JLabel labelEmpty4 = new JLabel("  ");
		
		MenuActionListener menuClick = new MenuActionListener();
		
		btnOpen = new JButton("open");
		btnOneStep = new JButton("실행(1 Step)");
		btnAllStep = new JButton("실행 (All)");
		btnExit = new JButton("종료");
		
		btnOpen.addActionListener(menuClick);
		btnOneStep.addActionListener(new ActionListener() {  // one step btn
			public void actionPerformed(ActionEvent arg0) { // ActionListener의 구체적인 내용 작성 
				if(!fileName.isEmpty() &&resourceManager.eFlag == 0) {
				oneStep(); // 실제 작업은 oneStep()함수에서 정의할 것.
				model1.addElement(sicSimulator.logList.get(count));

				if(!model2.contains(sicSimulator.instList.get(count))) {
					model2.addElement(sicSimulator.instList.get(count));
				}
				count++;
				update(); // 작업이 끝나면 visualSimulator 화면 업데이트
				}
			}
		});
		btnAllStep.addActionListener(new ActionListener() {  // all step btn
			public void actionPerformed(ActionEvent arg0) { // ActionListener의 구체적인 내용 작성 
				if(!fileName.isEmpty()) {
					allStep(); // 실제 작업은 allStep()함수에서 정의할 것.
					for(int i = count ; i < sicSimulator.logList.size(); i++) {
						model1.addElement(sicSimulator.logList.get(i));
					}
					for(int i = count; i < sicSimulator.instList.size();i++) {
						if(!model2.contains(sicSimulator.instList.get(i))) {
							model2.addElement(sicSimulator.instList.get(i));
						}
						count++;
					}
					
					update(); // 작업이 끝나면 visualSimulator 화면 업데이트
				}
			}
		});
		btnExit.addActionListener(new ActionListener() {  // 버튼이 눌렸을 때의 작동방식 선언. 여기서는 임시 ActionListener생성
			public void actionPerformed(ActionEvent arg0) { // ActionListener의 구체적인 내용 작성 
				resourceManager.closeDevice();
				System.exit(1);
			}
		});
		

		btnOpen.setBackground(Color.GRAY);
		btnOneStep.setBackground(Color.GRAY);
		btnAllStep.setBackground(Color.GRAY);
		btnExit.setBackground(Color.GRAY);

		nameOfFile = new JTextField(10);
		hProName = new JTextField(8);
		hStartAddr = new JTextField(6);
		hLength = new JTextField(6);
		regADec = new JTextField(); regAHex = new JTextField(); regXDec = new JTextField();
		regXHex = new JTextField(); regLDec = new JTextField(); regLHex = new JTextField();
		regPCDec = new JTextField(); regPCHex = new JTextField(); regSW = new JTextField();
		regBDec = new JTextField(); regBHex = new JTextField(); regSDec = new JTextField();
		regSHex = new JTextField(); regTDec = new JTextField(); regTHex = new JTextField();
		regF = new JTextField();
		eFirAddr = new JTextField(6); memStartAddr = new JTextField(12);
		targetAddr = new JTextField(); usingDev = new JTextField(6);
			
		log = new JList(model1);
		JScrollPane s = new JScrollPane(log);
		
		instructions = new JList(model2);
		JScrollPane ss = new JScrollPane(instructions);

		topPanel = new JPanel();
		topPanel.setBackground(Color.LIGHT_GRAY);
		topPanel.setLayout(new FlowLayout(5));
		topPanel.add(Filename);
		topPanel.add(nameOfFile);
		topPanel.add(btnOpen);
		mainContainer.add(topPanel, BorderLayout.NORTH);
		
		centerPanel = new JPanel();
		centerPanel.setBackground(Color.lightGray);
		centerPanel.setLayout(new GridLayout(1,2));
		mainContainer.add(centerPanel,BorderLayout.CENTER);
		//왼쪽 패널
		leftPanel = new JPanel();
		leftPanel.setBackground(Color.LIGHT_GRAY);
		leftPanel.setBorder(new LineBorder(Color.BLACK,2));
		leftPanel.setLayout(new GridLayout(3,1));
		leftPanel.setBorder(BorderFactory.createEmptyBorder(0,8,0,0));
		centerPanel.add(leftPanel);
		
		header = new JPanel(); headerLeft = new JPanel(); headerRight = new JPanel();
		header.setBackground(Color.lightGray);
		header.setBorder(new TitledBorder("H (Header Record)"));
		header.setLayout(new GridLayout(1,2));
		headerLeft.setBackground(Color.lightGray);
		headerRight.setBackground(Color.lightGray);
		
		header.add(headerLeft);
		header.add(headerRight);
		headerLeft.setLayout(new BoxLayout(headerLeft, BoxLayout.Y_AXIS));
		headerLeft.add(labelProName);
		headerLeft.add(labelEmpty);
		headerLeft.add(labelObjAddr);
		headerLeft.add(labelObjAddr2);
		headerLeft.add(labelEmpty2);
		headerLeft.add(labelHLength);

		headerRight.setLayout(null);
		headerRight.add(hProName);
		hProName.setBounds(5,0,60,20);
		headerRight.add(hStartAddr);
		hStartAddr.setBounds(5,45,50,20);
		headerRight.add(hLength);
		hLength.setBounds(5,80, 50, 20);
		leftPanel.add(header);
		
		register = new JPanel(); registerLeft = new JPanel(); registerRight = new JPanel();
		register.setBackground(Color.lightGray);
		registerLeft.setBackground(Color.lightGray);
		registerLeft.setLayout(new BoxLayout(registerLeft, BoxLayout.Y_AXIS));
		registerRight.setBackground(Color.lightGray);
		registerRight.setLayout(null);
		register.setLayout(new BorderLayout(0,0));
		register.setBorder(new TitledBorder("Register"));
		register.add(registerLeft, BorderLayout.WEST);
		register.add(registerRight,BorderLayout.CENTER);
		registerLeft.add(labelEmpty3);
		registerLeft.add(labelA);
		registerLeft.add(labelX);
		registerLeft.add(labelL);
		registerLeft.add(labelPC);
		registerLeft.add(labelSW);
		registerRight.add(labelDec); labelDec.setBounds(20,0,60,15);
		registerRight.add(labelHex); labelHex.setBounds(90,0,60,15);
		registerRight.add(regADec); regADec.setBounds(20,16,60,16);
		registerRight.add(regAHex); regAHex.setBounds(90,16,60,16);
		registerRight.add(regXDec); regXDec.setBounds(20,32,60,16);
		registerRight.add(regXHex); regXHex.setBounds(90,32,60,16);
		registerRight.add(regLDec); regLDec.setBounds(20,48,60,16);
		registerRight.add(regLHex); regLHex.setBounds(90,48,60,16);
		registerRight.add(regPCDec); regPCDec.setBounds(20,64,60,16);
		registerRight.add(regPCHex); regPCHex.setBounds(90,64,60,16);
		registerRight.add(regSW); regSW.setBounds(20,80,130,18);
		leftPanel.add(register);
		
		registerXE = new JPanel(); registerXELeft = new JPanel(); registerXERight = new JPanel();
		registerXE.setBackground(Color.LIGHT_GRAY);
		registerXELeft.setBackground(Color.LIGHT_GRAY);
		registerXERight.setBackground(Color.LIGHT_GRAY);
		registerXELeft.setLayout(new BoxLayout(registerXELeft, BoxLayout.Y_AXIS));
		registerXERight.setLayout(null);
		registerXE.setLayout(new BorderLayout(0,0));
		registerXE.setBorder(new TitledBorder("Register(for XE"));
		registerXE.add(registerXELeft, BorderLayout.WEST);
		registerXE.add(registerXERight,BorderLayout.CENTER);
		registerXELeft.add(labelEmpty4);
		registerXELeft.add(labelB);
		registerXELeft.add(labelS);
		registerXELeft.add(labelT);
		registerXELeft.add(labelF);
		registerXERight.add(labelDec2); labelDec2.setBounds(20,0,60,20);
		registerXERight.add(labelHex2); labelHex2.setBounds(90,0,60,20);
		registerXERight.add(regBDec); regBDec.setBounds(20,16,60,16);
		registerXERight.add(regBHex); regBHex.setBounds(90,16,60,16);
		registerXERight.add(regSDec); regSDec.setBounds(20,32,60,16);
		registerXERight.add(regSHex); regSHex.setBounds(90,32,60,16);
		registerXERight.add(regTDec); regTDec.setBounds(20,48,60,16);
		registerXERight.add(regTHex); regTHex.setBounds(90,48,60,16);
		registerXERight.add(regF); regF.setBounds(20,64,130,16);
		leftPanel.add(registerXE);
		//오른쪽 패널
		rightPanel = new JPanel();
		rightPanel.setBackground(Color.lightGray);
		rightPanel.setLayout(null);
		rightPanel.setBorder(BorderFactory.createEmptyBorder(0,10,0,0));
		
		endPanel = new JPanel(); 
		endPanel.setBounds(10,0,220,80);
		endPanel.setBackground(Color.LIGHT_GRAY);
		endPanel.setBorder(new TitledBorder("E (End Record)"));
		endPanel.setLayout(null);
		endPanel.add(labelFirAddr); labelFirAddr.setBounds(15,15,160,20);
		endPanel.add(labelFirAddr2); labelFirAddr2.setBounds(30,35,120,20);
		endPanel.add(eFirAddr); eFirAddr.setBounds(150,40,60,20);
		rightPanel.add(endPanel);
		
		addrPanel = new JPanel();
		addrPanel.setBounds(10,90,220,80);
		addrPanel.setBackground(Color.LIGHT_GRAY);
		addrPanel.setLayout(null);
		addrPanel.add(labelStartAddr); labelStartAddr.setBounds(5,5,150,20);
		addrPanel.add(memStartAddr); memStartAddr.setBounds(100,25,100,20);
		addrPanel.add(labelTargetAddr); labelTargetAddr.setBounds(5,45,150,20);
		addrPanel.add(targetAddr); targetAddr.setBounds(100,48,100,20);
		rightPanel.add(addrPanel);
		
		instPanel = new JPanel(); instPanelLeft = new JPanel(); instPanelRight= new JPanel();
		instPanelBtn = new JPanel(); instPanelDev = new JPanel();
		instPanel.setBounds(10, 170,240,250);
		instPanel.setBackground(Color.magenta);
		instPanelLeft.setBackground(Color.LIGHT_GRAY);
		instPanelLeft.setLayout(new BorderLayout(0,0));
		instPanelRight.setBackground(Color.LIGHT_GRAY);
		instPanelRight.setLayout(new GridLayout(2,1));
		instPanel.setLayout(new GridLayout(1,2));
		
		instPanelLeft.add(labelInst,BorderLayout.NORTH);
		instPanelLeft.add(ss, BorderLayout.CENTER);
		instPanelLeft.setBorder(BorderFactory.createEmptyBorder(0,0,0,10));
		instPanelRight.add(instPanelDev);
		instPanelRight.add(instPanelBtn);
		instPanelDev.setBackground(Color.LIGHT_GRAY);
		instPanelBtn.setBackground(Color.LIGHT_GRAY);
		
		instPanelDev.add(labelDev); 
		instPanelDev.add(usingDev);
		
		btnOneStep.setPreferredSize(new Dimension(100,20));
		btnAllStep.setPreferredSize(new Dimension(100,20));
		btnExit.setPreferredSize(new Dimension(100,20));
		
		
		instPanelBtn.add(btnOneStep);
		instPanelBtn.add(btnAllStep); 
		instPanelBtn.add(btnExit); 
		
		instPanel.add(instPanelLeft);
		instPanel.add(instPanelRight);
		
		rightPanel.add(instPanel);
		centerPanel.add(rightPanel);
		
		
		bottomPanel = new JPanel();
		bottomPanel.setBackground(Color.lightGray);
		bottomPanel.setBorder(new LineBorder(Color.BLACK,2));
		bottomPanel.setBorder(BorderFactory.createEmptyBorder(2,10,10,10));
		bottomPanel.setLayout(new BorderLayout(0,0));
		bottomPanel.add(labelLog,BorderLayout.NORTH);
		bottomPanel.add(s);
		mainContainer.add(bottomPanel,BorderLayout.SOUTH);
	}
	
	class MenuActionListener implements ActionListener{
		public void actionPerformed(ActionEvent e) {
			String cmd = e.getActionCommand();
			switch(cmd) {
			case "open":
				JFileChooser chooser = new JFileChooser(); // open할 파일을 선택할 때 JfileChooser를 쓰는 방식 예제

				int ret = chooser.showOpenDialog(null);
				if( ret == JFileChooser.APPROVE_OPTION){
					try {
						load(chooser.getSelectedFile());
						update();
					} catch (Exception a) {
						a.printStackTrace();
					}
					fileName = chooser.getSelectedFile().getName();
				}
				
				break;
			}
		}
	}
	
	/**
	 * 프로그램 로드 명령을 전달한다.
	 */
	public void load(File program){
		//...
		sicLoader.load(program);
		sicSimulator.load(program);
	};

	/**
	 * 하나의 명령어만 수행할 것을 SicSimulator에 요청한다.
	 */
	public void oneStep(){
		sicSimulator.oneStep();
	};

	/**
	 * 남아있는 모든 명령어를 수행할 것을 SicSimulator에 요청한다.
	 */
	public void allStep(){
		sicSimulator.allStep();
	};
	
	public String decToHex(int dec) {	//10진수를 16진수 문자열로 바꿔주는 함수
		String tempHex = new String();
		String temp = new String();
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
			temp = String.valueOf(tempHex);
		}
		return temp;
	}
	
	public void update(){		//갱신하는 함수
		int length = 0;
		nameOfFile.setText(this.fileName);
		hProName.setText(sicLoader.title.get(0));
		hStartAddr.setText(sicLoader.startAddr);
		usingDev.setText(resourceManager.usingDev);
		for(int i = 0; i < sicLoader.forLength.size();i++) {
			length += sicLoader.forLength.get(i);
		}
		hLength.setText(Integer.toHexString(length).toUpperCase());
		
		regLDec.setText(String.valueOf(resourceManager.getRegister(2)));
		regLHex.setText(decToHex(resourceManager.getRegister(2)));
		
		regADec.setText(String.valueOf(resourceManager.getRegister(0)));
		regAHex.setText(decToHex(resourceManager.getRegister(0)));
		
		regXDec.setText(String.valueOf(resourceManager.getRegister(1)));
		regXHex.setText(decToHex(resourceManager.getRegister(1)));
		
		regPCDec.setText(String.valueOf(resourceManager.getRegister(8)/2));
		regPCHex.setText(decToHex(resourceManager.getRegister(8)/2));
		
		regSW.setText(decToHex(resourceManager.getRegister(9)));
		
		regBDec.setText(String.valueOf(resourceManager.register[3]));
		regBHex.setText(decToHex(resourceManager.register[3]));
		
		regSDec.setText(String.valueOf(resourceManager.register[4]));
		regSHex.setText(decToHex(resourceManager.register[4]));
		
		regTDec.setText(String.valueOf(resourceManager.register[5]));
		regTHex.setText(decToHex(resourceManager.register[5]));
		
		regF.setText(decToHex((int)resourceManager.register_F));
		
		eFirAddr.setText(sicLoader.eStartAddr);
		memStartAddr.setText(String.valueOf(sicLoader.mStartAddr));
		targetAddr.setText(decToHex(SicSimulator.targetAddr));
		
		log.setModel(model1);
		instructions.setModel(model2);
		
	};
	
	public static void main(String[] args) {
		VisualSimulator VS = new VisualSimulator("Sic/XE Simulator");
		VS.setVisible(true);
	}
}
