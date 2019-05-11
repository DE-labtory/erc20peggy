pragma solidity ^0.5.0;

import "./ERC721.sol";
import "./AuctionBase.sol";

contract Auction is AuctionBase {

    bool public isAuction = true;

    // TODO: set InterfaceSignature
    bytes4 constant InterfaceSignature_ERC721 = bytes4(0x9a20483d);

    function Auction(address _nftAddress){

        ERC721 candidateContract = ERC721(_nftAddress);
        require(candidateContract.supportsInterface(InterfaceSignature_ERC721));
        nonFungibleContract = candidateContract;
    }

    function createAuction(uint256 _tokenId, uint256 _duration) external {
        require(_duration == uint256(uint64(_duration)));

        require(msg.sender == address(nonFungibleContract));
        require(_owns(msg.sender, _tokenId));
        _escrow(msg.sender, _tokenId);
        // TODO: set start price (max price)
        Auction memory auction = Auction(msg.sender, address(0), uint128(0), uint256(0), uint64(_duration), uint64(now));
        _addAuction(_tokenId, auction);
    }

    function bid(uint256 _tokenId) external payable {
        require(msg.sender == address(nonFungibleContract));

        _bid(_tokenId, msg.value);
    }

    // TODO: return ether to bidders
    // TODO: set authority (owner or seller)
    function cancelAuction(uint256 _tokenId) external {
        Auction storage auction = tokenIdAuction[_tokenId];
        require(_isOnAuction(auction));

        _cancelAuction(_tokenId, seller);
    }

    function getAuction(uint256 _tokenId) external view returns (address, address, uint256, uint256, uint256, uint256) {

        Auction storage auction = tokenIdToAuction[_tokenId];
        require(_isOnAuction(auction));

        return (auction.seller, auction.winner, auction.maxPrice, auction.sumPrice, auction.duration, auction.startedAt);
    }

    function getBidder(uint256 _tokenId) external view returns (uint256) {
        Auction storage auction = tokenIdToAuction[_tokenId];
        require(_isOnAuction(auction));

        require(tokenIdToBidderAndAmount[_tokenId][msg.sender] != 0);

        return tokenIdToBidderAndAmount[_tokenId][msg.sender];
    }

    function isFinished(uint256 _tokenId) external returns (bool) {
        return _isFinished(_tokenId);
    }

    // TODO: send ether to winner
    function endAuction(uint256 _tokenId) external payable {
        require(_isFinished(_tokenId));

        Auction storage auction = tokenIdToAuction[_tokenId];

        _transfer(auction.winner, _tokenId);
        emit AuctionSuccessful(_tokenId, auction.sumPrice, auction.winner);

        _removeAuction(_tokenId);
    }
}
