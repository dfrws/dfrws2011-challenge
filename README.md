# dfrws2011-challenge
Given the variety and impending ubiquity of Android devices along with the wide range of crimes that can involve these systems as a source of evidence, the DFRWS has created two scenarios for the forensics challenge in 2011.

## The Winners

Fox-IT in the Neatherlands won the challenge. They submitted Python utilities they developed, presented visual reconstruction of evidence, and had a Great overall synthesis of evidence and application to the Scenario.

## Scenario 1: Suspicious Death

Donald Norby was found dead in his home with a single bullet to the head. It is unclear whether this is a suicide or homicide. The largest question revolves around the victim's potential connections to an organized criminal group called KRYPTIX. You have been asked to perform a forensic examination of Norby's Android device found at the scene in order to determine his activities and, possibly, who he communicated with prior to his death. Your ultimate goal is to determine whether he killed himself or was murdered and provide any further leads to the investigator.

The device was acquired using what the agent considered to be industry best practices. The device flash storage as well as removable media was collected. See the case specific logs for more information. The files are provided in a 157 MB tgz file Case1.tgz. Note this file uncompresses to be roughly 16.5GB.

- [Case1.tgz](http://old.dfrws.org/2011/challenge/Case1.tgz)- MD5: 9a756c41cbd3b628fb55d35e695efdee31efa58e *too big for github*

## Scenario 2: Intellectual Property Theft

A serious breach of security occurred within an organization named SwiftLogic Inc.; valuable documents containing designs of a new product named Palomino were confirmed to have been leaked to a competitor. Based on an internal investigation, Yob Taog is suspected of the leak and was suspended pending investigation. Taog's Android smartphone was - surprisingly - voluntarily submitted by Taog for further investigation. Your goal in this investigation is to document any evidence that intellectual property was stolen, to support termination of Yob Toag and potential criminal charges. You are asked to perform a forensic analysis of his device for evidence of the breach.

The device was acquired using what the agent considered to be industry best practices. The device flash storage as well as removable media was collected. See the case specific logs for more information. The files are provided in a 338 MB tgz file Case2.tgz. Note this file uncompresses to be roughly 16.5GB 

- [Case2.tgz](http://old.dfrws.org/2011/challenge/Case2.tgz) - MD5: 17bd6109410a0c57439aa8e701354a5f1dfd4ab3  *too big for github*

## Submission Requirements
Submissions should include a detailed analysis in report format that:

- Addresses the specific goals within each scenario.
- Provides a reconstruction and timeline of relevant events on each device.
- Highlights any leads or new evidence that would advance or widen the investigations.
- State an opinion or your findings on the following evidentiary issues:
  - Was the device "rooted"?
  - Did the device have "adb" enabled?
  - Was the collection process sound? (and can you verify the recorded acquisition log)
- List applications installed
- Indicate version information
- Recover credentials, if possible

The report should also include any other conclusions that appear germane to the case and must outline novel techniques employed in sufficient detail that the results can be reproduced. Reports must be submitted in PDF or ASCII format. The submission should also include data that supports the findings and the source code for any analysis tools that were developed for the challenge. The source code can be released under any restrictions and licenses that you choose. The report and supporting files should be bundled into a single compressed archive. All submitted data, with the exception of compiled executables, will be published on the DFRWS website.

Submissions were due by July 24, 2011.

## Criteria

Submissions will be judged primarily for the completeness and accuracy of findings, as well as the soundness of the supporting analysis. The goal of this and past challenges is to spur advances in the state of the art in research and tools. Therefore, we expect that you document your techniques as much as possible. Extra weight will be given for the creation of novel analysis tools that are applicable to broader forensic challenges.

## Acknowledgements

The DFRWS would like to thank Tim Vidas from Carnegie Mellon University for performing the technical aspects of the scenarios and acquiring data from the devices. Eoghan Casey, Matthew Geiger and Tim Vidas developed the challenge scenarios and coordinated efforts to execute the scenarios.
