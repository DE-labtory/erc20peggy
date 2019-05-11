pragma solidity ^0.5.0;

import "./GhostOwnership.sol";
import "./Auction.sol";

contract GhostAuction is GhostOwnership {

    // TODO: set authority of calling this method
    function setAuctionAddress(address _contractAddress) external {
        Auction candidateContract = Auction(_contractAddress);

        require(candidateContract.isAuction);

        candidateAuctionContract = candidateContract;
    }

    function createAuction(uint256 _tokenId) external {
        require(_owns(msg.sender, _tokenId));

        // TODO: initiate duration
        uint256 _duration = 0;

        _approve(_tokenId, candidateAuctionContract);

        candidateAuctionContract.createAuction(_tokenId, _duration);
    }
}
