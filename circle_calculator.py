#功能改进v1.4
import time,random
def pBar(start = 0,end = 100,aSW = False,pBarE = None,bar_len = 50):
    if start == end:
        print("Error: start and end cannot be the same value")
        return    
    if start > end:           #防止后续调用时，铸币用户不小心把s和e的值写反了，虽然不太可能会有后续调用
        start,end = end,start
    if pBarE is None:
        pBarE = end
    for i in range(start,end+1):
        filled = int(bar_len * (i / pBarE))
        if filled > bar_len:
            filled = bar_len
        bar ='-' * filled + ' ' * (bar_len - filled)
        print(f"\r{i}%|{bar}||This progress bar is fake,just fot test,do not mind",end ="",flush = True)
        time.sleep(0.02)
    if not aSW:
        print('')

def fm(r,PI = 3.1415927):  #默认Pi为单精度Pi值
    s = r**2 * PI
    return s

def chkIll(txt):#“非法性”检查
    pSW = False
    poSW = False
    sSW = False
    if len(txt) > 1000:                     #长度检查
        print ("Error: input exceeds 1000 characters")
        return True
    if not txt:                             #空输入检查
        return True
    if txt[-1] in "eE+-":                   #尾字符检查，此方法避免了出现IndexError的情况
        return True
    if txt[-1] == ".":                      #用来放行5.这种情况
        if len(txt) > 1:
            if txt[-2] not in "1234567890": 
                return True
        else:
            return True
    k = 0
    for ch in txt:                          #非法字符检查
        if ch not in "1234567890.eE+-":
            print("Error: invalid character detected")
            return True
        else:
            if ch == ".":                    #小数点检查
                if pSW or sSW:
                    return True
                else:
                    pSW = True
            if ch in "+-":                   #正负号检查
                if poSW:
                        return True                            
                else:
                    if k == 0:
                        if not (ch == "+" and len(txt) >1):
                            if ch == "-":
                                print("Error: only positive numbers allowed")
                            return True
                    else:
                        if txt[k-1] not in "eE":
                            return True
                        else:
                            poSW = True
            if ch in "Ee":                   #科学计数法检查
                if sSW:
                    return True
                else:
                    if k == 0 or k+1 == len(txt):
                        return True
                    else:
                        if not (txt[k - 1].isdigit() or (txt[k - 1] == "." and k - 2 >= 0 and txt[k - 2].isdigit())):
                            return True        
                        if txt[k+1] not in "1234567890+-":
                            return True
                        sSW = True
        k += 1
    return False

def main_loop():
    while True:
        i = input("Enter radius (positive number, or 'e' to exit): ").strip(" ")    #此处使用.strip(" ")而不是.strip()，因为程序不能预判用户的目的
        if i.lower().strip() == "e":
            return
        if i.lower().strip() == "h":        #.strip()防止铸币用户忘记自己多打了空格，与上面不同是因为此处应该体谅用户“退出”的目的
            print("\nWhat can this program do?\n1. Calculate the area of a circle based on the radius you input.\n2. That's it.\n\nYes! It's indeed quite boring, because this program is essentially a test harness I use to understand test state machines.\n\nThis program will intercept the following inputs:\n- Non-numeric input\n- Malformed scientific notation\n- Non-positive numbers\n- Mathematical expressions\n\nNote: Inputs in non-decimal bases are not supported.\nWell,you may say 'why not use try..expect?'yeah,u r right,it's quite simple and short\nBut,if you never try to reinvent the wheel, how can you understand it? \n")
            continue
        while chkIll(i):
            i = input("Invalid input.please try again:").strip(" ")
            if i.lower().strip() == "e":    #同上
                return
        n = fm(float(i))
        p = random.randint(0,100)
        if p > 25 and p < 75:
            pBar(0,p,True,100,50)
            print("\r" + ' ' * 200,end = "",flush = True)       #清空字符
            print("\rsorry,progress bar is broken,we are tring to fix",end = "",flush = True)
            time.sleep(1.5)
            print("\r" + ' ' * 200,end = "",flush = True)       #同上
            print("\rfix cpmplete,progress bar is working now",end = "",flush = True)
            time.sleep(1.5)
            pBar(p,100,False,100,50)
        else:
            pBar()
        print("S=",n)
        
print("this is a State Machine test program,the function is a Circle Area Calculator (radius → area)")
main_loop()
print("Thanks for using.Have a nice day!")
