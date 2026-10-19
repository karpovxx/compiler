function tokenize(source) {
    let tokens = [];
    let pos = 0;
    const keywords = ["int", "if", "else", "while", "return"];

    while (pos < source.length) {
        let char = source[pos];
        if (/\s/.test(char)) { pos++; continue; }

        if (/[a-zA-Z]/.test(char)) {
            let id = "";
            while (pos < source.length && /[a-zA-Z0-9_]/.test(source[pos])) id += source[pos++];
            tokens.push({ type: keywords.includes(id) ? "KEYWORD" : "IDENT", value: id });
        } 
        else if (/[0-9]/.test(char)) {
            let num = "";
            while (pos < source.length && /[0-9]/.test(source[pos])) num += source[pos++];
            tokens.push({ type: "NUMBER", value: num });
        } 
        else if ("+-*/=<>".includes(char)) {
            let op = char; pos++;
            if (char === "=" && source[pos] === "=") op += source[pos++];
            tokens.push({ type: "OPERATOR", value: op });
        } 
        else if (char === "(") { tokens.push({ type: "LPAREN", value: "(" }); pos++; }
        else if (char === ")") { tokens.push({ type: "RPAREN", value: ")" }); pos++; }
        else if (char === "{") { tokens.push({ type: "LBRACE", value: "{" }); pos++; }
        else if (char === "}") { tokens.push({ type: "RBRACE", value: "}" }); pos++; }
        else if (char === ";") { tokens.push({ type: "SEMICOLON", value: ";" }); pos++; }
        else { tokens.push({ type: "UNKNOWN", value: char }); pos++; }
    }
    tokens.push({ type: "END", value: "" });
    return tokens;
}

let tokens = [];
let pos = 0;

function peek() { return tokens[pos]; }
function consume() { return tokens[pos++]; }
function expect(type, val = "") {
    if (peek().type === type && (val === "" || peek().value === val)) return consume();
    throw new Error("Syntax error at token: " + peek().value);
}

function parseProgram() {
    let stmts = [];
    while (peek().type !== "END") stmts.push(parseStatement());
    return stmts;
}

function parseStatement() {
    if (peek().type === "KEYWORD" && peek().value === "int") consume();
    if (peek().type !== "IDENT") throw new Error("Expected variable name, got: " + peek().value);
    let varName = consume().value;
    expect("OPERATOR", "=");
    let expr = parseExpression();
    expect("SEMICOLON");
    return { type: "Assign", var: varName, expr: expr };
}

function parseExpression() {
    let left = parseTerm();
    while (peek().type === "OPERATOR" && ["+", "-", "==", "<", ">"].includes(peek().value)) {
        let op = consume().value;
        left = { type: "BinaryOp", op: op, left: left, right: parseTerm() };
    }
    return left;
}

function parseTerm() {
    let left = parseFactor();
    while (peek().type === "OPERATOR" && ["*", "/"].includes(peek().value)) {
        let op = consume().value;
        left = { type: "BinaryOp", op: op, left: left, right: parseFactor() };
    }
    return left;
}

function parseFactor() {
    if (peek().type === "NUMBER") return { type: "Number", value: consume().value };
    if (peek().type === "IDENT") return { type: "Var", name: consume().value };
    if (peek().type === "LPAREN") {
        consume();
        let expr = parseExpression();
        expect("RPAREN");
        return expr;
    }
    throw new Error("Unexpected token: " + peek().value);
}

function printAST(node, indent = 0) {
    let space = " ".repeat(indent);
    if (node.type === "Number") return space + `Number(${node.value})\n`;
    if (node.type === "Var") return space + `Var(${node.name})\n`;
    if (node.type === "BinaryOp") return space + `BinaryOp(${node.op})\n` + printAST(node.left, indent + 2) + printAST(node.right, indent + 2);
    if (node.type === "Assign") return space + `Assign(${node.var})\n` + printAST(node.expr, indent + 2);
    return "";
}

let tacCode = [];
let tempCounter = 0;
function newTemp() { return "t" + (++tempCounter); }

function generateTAC(node) {
    if (node.type === "Number") {
        let t = newTemp();
        tacCode.push({ op: "=", arg1: node.value, arg2: "", result: t });
        return t;
    }
    if (node.type === "Var") return node.name;
    if (node.type === "BinaryOp") {
        let left = generateTAC(node.left);
        let right = generateTAC(node.right);
        let t = newTemp();
        tacCode.push({ op: node.op, arg1: left, arg2: right, result: t });
        return t;
    }
    if (node.type === "Assign") {
        let exprResult = generateTAC(node.expr);
        if (exprResult !== node.var) tacCode.push({ op: "=", arg1: exprResult, arg2: "", result: node.var });
        return node.var;
    }
}

function printTAC(code) {
    return code.map(inst => {
        if (inst.op === "=") return `${inst.result} = ${inst.arg1}`;
        return `${inst.result} = ${inst.arg1} ${inst.op} ${inst.arg2}`;
    }).join("\n");
}

function optimizeTAC(code) {
    let constants = {};
    for (let inst of code) {
        if (inst.op === "=") {
            if (constants[inst.arg1]) inst.arg1 = constants[inst.arg1];
            if (!isNaN(parseInt(inst.arg1))) constants[inst.result] = inst.arg1;
            else delete constants[inst.result];
        } else {
            if (constants[inst.arg1]) inst.arg1 = constants[inst.arg1];
            if (constants[inst.arg2]) inst.arg2 = constants[inst.arg2];
            
            if (!isNaN(parseInt(inst.arg1)) && !isNaN(parseInt(inst.arg2))) {
                let v1 = parseInt(inst.arg1), v2 = parseInt(inst.arg2), res = 0;
                if (inst.op === "+") res = v1 + v2;
                else if (inst.op === "-") res = v1 - v2;
                else if (inst.op === "*") res = v1 * v2;
                else if (inst.op === "/") res = Math.floor(v1 / v2);
                
                inst.op = "=";
                inst.arg1 = res.toString();
                inst.arg2 = "";
                constants[inst.result] = inst.arg1;
            }
        }
    }

    if (code.length === 0) return code;
    let used = new Array(code.length).fill(false);
    used[code.length - 1] = true;

    for (let i = code.length - 1; i >= 0; i--) {
        if (!used[i]) continue;
        for (let j = i - 1; j >= 0; j--) {
            if (code[j].result === code[i].arg1 || code[j].result === code[i].arg2) used[j] = true;
        }
    }

    return code.filter((_, index) => used[index]);
}

function compile() {
    const source = document.getElementById("sourceCode").value;
    
    document.getElementById("astOutput").textContent = "";
    document.getElementById("tacBefore").textContent = "";
    document.getElementById("tacAfter").textContent = "";
    document.getElementById("astOutput").className = "";

    try {
        tokens = tokenize(source);
        pos = 0;

        let ast = parseProgram();
        
        let astStr = "";
        for (let node of ast) astStr += printAST(node);
        document.getElementById("astOutput").textContent = astStr;

        tacCode = [];
        tempCounter = 0;
        for (let node of ast) generateTAC(node);
        document.getElementById("tacBefore").textContent = printTAC(tacCode);

        let optimized = optimizeTAC(tacCode);
        document.getElementById("tacAfter").textContent = printTAC(optimized) || "(пусто)";

    } catch (e) {
        document.getElementById("astOutput").textContent = "Ошибка: " + e.message;
        document.getElementById("astOutput").className = "error";
    }
}