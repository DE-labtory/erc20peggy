# Solidity grammar for dApp

##### Solidity code is located in contract. Contract is a fundamental component of the Ethereum application and all variables, functions are within the contract.

- **Contract Structure**

  > pragma solidity ^<version>; 
  >
  > contract <name> { 
  >
  > ​	// Include variable, function, modifier, event, structures, enumerations … 
  >
  > } 

- **Grammar** 

  - **Data type ( bool, uint, fixed, address, …)**

    > uint a = 10;
    >
    > bool b = true;
    >
    > address c = msg.sender;

  - **Mapping ( key & value )**

    > mapping (uint => address) mapVar;

  - **Structure**

    > struct Token {
    >
    > ​	string name;
    >
    > ​	uint dna;
    >
    > }

  - **Array**

    > // fixed array 
    >
    > uint[2] fixedArray;
    >
    > // dynamic array
    >
    > int[] dynamicArray;
    >
    > // struct array
    >
    > Token[] tokens; 

  - **Memory**

    > // temporary storage variable
    >
    > string memory name;

  - **Storage**

    > // Permanent storage variable
    >
    > string storage name;

  - **Access modifier**

    > // public, private, internal, external
    >
    > Token[] public tokens;
    >
    > function callInternal() internal { }
    >
    > function generateRandom() private { }
    >
    > function callExternal() external { }

  - **State modifier**

    > // view : can access to data but can't change. Burn no gas.
    >
    > function usingView() private view returns (uint) {
    >
    > ​	return tokenCount[0];
    >
    > }
    >
    > // pure : can't access any data. Also burn no gas
    >
    > function usingPure(uint a, uint b) private pure returns (uint) {
    >
    > ​	return a * b;
    >
    > }

  - **User-defined modifier**

    > modifier onlyOnwer() { }

  - **Payable modifier**

    > // Type of function that can receive ether.
    >
    > // Pay for contract at the same time when execute function
    >
    > function payable_function() public payable returns (uint) { }

  - **Event**

    > // event is a method of communicating at the user level when action occurs.
    >
    > event NewToken(uint tokenId, string name, uint dna);

  - **Balance, value, sender**

    > // msg.value is to check how many ether has been sent to contract.
    >
    > // this.balance is the total balance stored in the contract.
    >
    > // msg.sender is the variable that points to the address who calls the current function

- **Gas Saving**

  - Gas must be paid for storage space, computation processing.

  - Solidity defaults to size 256-bit storage space.

  - Structure compression is required to save gas. 

    > struct Compression {
    >
    > ​	uint8 a;
    >
    > ​	uint8 b;
    >
    > ​	uint32 c;
    >
    > ​	uint32 d;
    >
    > ​	uint256 e;
    >
    > ​	uint256 f;
    >
    > }

- **Token**
  
- Token is a smart contract that follows the basic conventions.
  
- **Comment**
  - @notice : explain to the user what the function and contract is doing
  - @param : explain what parameters have in a function
  - @return : explain what return values have in a function
  - @dev : explain additional details