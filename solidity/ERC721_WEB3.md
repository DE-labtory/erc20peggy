## Solidity Contract with cryptozombie

- **TokenFactory**

  This contract generates random Tokens.

  > ```
  > contract TokenFactory is Ownable {
  >     // event
  >     event NewToken(uint tokenId, string name, uint dna);
  >     
  >     struct Token {
  >         string name;
  >         uint dna;
  >     }
  >     Token[] public tokens;
  > 
  >     mapping (uint => address) public tokenToOwner;
  >     mapping (address => uint) ownerTokenCount;
  > 
  >     // creating Token
  >     function createRandomToken(string memory _name) public {
  >     		generateRandomToken();
  >         _createToken();
  >     }
  > 
  >     function _createToken(string memory _name, uint _dna) internal returns (uint) {}
  >     function generateRandomToken(string memory _name) private view returns (uint) {}
  > }
  > ```



- **Ownable**

  This contract has an owner address, and provides basic authorization control functions, this simplifies the implementation of "user permissions".

  > ```
  > contract Ownable {
  >     address private _owner;
  >     constructor () internal {
  >         _owner = msg.sender;
  >     }
  > 
  >     function owner() public view returns (address) {
  >         return _owner;
  >     }
  > 
  >     modifier onlyOwner() {
  >         require(msg.sender == _owner, "Ownable: caller is not the owner");
  >         _;
  >     }
  > 
  >     function _transferOwnership(address newOwner) public onlyOwner {
  >         require(newOwner != address(0), "Ownable: new owner is the zero address");
  >         _owner = newOwner;
  >     }
  > }
  > ```



- **TokenOwnership**

  This contract implements ERC721.

  > ```
  > contract TokenOwnership is TokenFactory, ERC721 {
  >     using Safemath for uint;
  >     /*** ERC 721 implementation ***/
  >     event Transfer(address indexed from, address indexed to, uint256 indexed tokenId);
  >     event Approval(address indexed owner, address indexed approved, uint256 indexed tokenId);
  > 
  >     mapping (uint => address) approvals;
  > 
  >     function balanceOf(address owner) public view returns (uint256 balance) {
  >         // return ownerTokenCount[owner]
  >     }
  >     function ownerOf(uint256 tokenId) public view returns (address) {
  >         // return tokenToOwner[tokenId];
  >     }
  >     function transfer(address _to, uint256 _tokenId) public {
  >         // check if it's possible, call _transfer
  >     }
  >     function approve(address _to, uint256 _tokenId) public {
  >         // store token in new map, emit Approval event
  >     }
  >     function takeOwnership(uint256 _tokenId) public {
  >         // check if it's possible, call _transfer
  >     }
  >     function _transfer(address _from, address _to, uint256 _tokenId) private {
  >         // add 1 to receiver, sub 1 from owner, update token address, emit event
  >     }
  > }
  > ```



## ERC 721

**ERC-721 is a free, open standard that describes how to build non-fungible or unique tokens on the Ethereum blockchain.** 

- **Cautions for Implementation**
  - Overflow & underflow management
  - Use SafeMath Library when needed ( assert )
  - Unlike the require, assert doesn't return the remaining gas to the user when the event of the fuction fails

- **Functions**

  > ```
  > event Transfer(address indexed from, address indexed to, uint256 indexed tokenId);
  > event Approval(address indexed owner, address indexed approved, uint256 indexed tokenId);
  > 
  > // return amount of token that address have
  > function balanceOf(address _owner) public view returns (uint256 _balance);
  > 
  > // return the address of the person who owns the token
  > function ownerOf(uint256 _tokenId) public view returns (address);
  > 
  > // transfer the ownership on the owner's side
  > function transfer(address _to, uint256 _tokenId) public;
  > 
  > // transfer the ownership on the recipient side
  > function approve(address _to, uint256 _tokenId) public;
  > function takeOwnership(uint256 _tokenId) public;
  > ```



## SafeMath

**SafeMath Library is to prevent overflow and underflow.**

- **Usage**

  > using SafeMath for uint;	// 8, 16, 32 ...



## Web3 

**The web3.js library is a collection of modules which contain specific functionality for the ethereum ecosystem.** 

- **Installation (Assume that npm is installed)**

  > mkdir <directory> 
  >
  > cd <directory>
  >
  > npm init
  >
  > npm install express —save 
  >
  > npm install web3

  

- **Using web3.js to access the Ethereum node**

  > var Web3 = require('web3')
  > var web3 = new Web3(new Web3.providers.HttpProvider("http://localhost:#portNumber"))

  

- **Address & ABI**

  Web3.js need address and abi to communicate with smart contract. You can get address and abi when you deploy the contract. ABI is short for Application Binary Interface. It's basically a JSON form that show the method of the contract. You can get abi when you compile the contract.

  You can instantiate your contract in Web3.js

  > var myContract = new web3js.eth.Contract(ABI, ContractAddress)

- **Web3.js function (call & send)**

  Call is used for view and pure function. View and Pure function is read-only and don't change the state of the blockchain, so call runs only on the local nodes and doesn't make transactions in blockchain network

  > myContract.methods.MethodName(param).call()

  Send is used for all function except view and pure. Send makes transaction and change data in blockchain. Sending a transaction requires the user to pay for the gas

  > myContract.methods.MethodName(param).send()

  